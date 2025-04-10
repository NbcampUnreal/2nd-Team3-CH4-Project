#include "WeaponMasterGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemTypes.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Containers/Array.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

UWeaponMasterGameInstance::UWeaponMasterGameInstance()
{
	
}

// Login
void UWeaponMasterGameInstance::Login()
{
	if (IsRunningDedicatedServer()) return; // 서버는 따로 로그인 안함

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	
	const FUniqueNetIdPtr NetId = Identity->GetUniquePlayerId(0);
	if (NetId != nullptr && Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn) return;
	
	LoginDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle( // 핸들러 등록
		0,
		FOnLoginCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleLoginCompleted));

	FString AuthType;
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), AuthType);

	if (!AuthType.IsEmpty()) // 테스트용 DevAuth 계정
	{
		UE_LOG(LogTemp, Log, TEXT("Logging into EOS..."));
      
		if (!Identity->AutoLogin(0))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to login... "));
			Identity->ClearOnLoginCompleteDelegate_Handle(0, LoginDelegateHandle);
			LoginDelegateHandle.Reset();
		}
	}
	else
	{
		FOnlineAccountCredentials Credentials("AccountPortal","", "");

		UE_LOG(LogTemp, Log, TEXT("Logging into EOS..."));
        
		if (!Identity->Login(0, Credentials))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to login... "));

			Identity->ClearOnLoginCompleteDelegate_Handle(0, LoginDelegateHandle);
			LoginDelegateHandle.Reset();
		}
	}
}

void UWeaponMasterGameInstance::HandleLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	
	const FUniqueNetIdPtr NetId = Identity->GetUniquePlayerId(0);
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Login callback completed!"));
		UE_LOG(LogTemp, Log, TEXT("Loading cloud data and searching for a session..."));

		OnProcessReturnValue.Broadcast(EPlayerEOSStateType::Login, ESessionResultType::Success);
		FindSessions();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EOS login failed."));
		// 로그인 실패시 EOS 게임모드 통해 메인메뉴로 이동
	}

	Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginDelegateHandle);
	LoginDelegateHandle.Reset();
}

void UWeaponMasterGameInstance::FindSessions(FName SearchKey, FString SearchValue)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	TSharedRef<FOnlineSessionSearch> Search = MakeShared<FOnlineSessionSearch>();

	Search->QuerySettings.SearchParams.Empty();
	Search->bIsLanQuery = false;
	Search->QuerySettings.Set(SearchKey, SearchValue, EOnlineComparisonOp::Equals);

	FindSessionsDelegateHandle =
		Session->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleFindSessionsCompleted,
			Search));

	UE_LOG(LogTemp, Log, TEXT("Finding session."));

	if (!Session->FindSessions(0, Search))
	{
		UE_LOG(LogTemp, Error, TEXT("Finding session failed."));
        
		Session->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsDelegateHandle);
		FindSessionsDelegateHandle.Reset();
	}
}

void UWeaponMasterGameInstance::HandleFindSessionsCompleted(bool bWasSuccessful, TSharedRef<FOnlineSessionSearch> Search)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	if (bWasSuccessful)
	{
		if (Search->SearchResults.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Finding session failed."));
			return;
		}
        
		UE_LOG(LogTemp, Warning, TEXT("Found session."));
        
		for (auto SessionInSearchResult : Search->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Session OwningUserName [%s]"), *SessionInSearchResult.Session.OwningUserName);
            
			if (Session->GetResolvedConnectString(SessionInSearchResult, NAME_GamePort, ConnectString))
			{
				SessionToJoin = &SessionInSearchResult;
			}
			break;
		}

		OnProcessReturnValue.Broadcast(EPlayerEOSStateType::FindSession, ESessionResultType::Success);
		JoinSession();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find session failed."));
	}

	Session->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsDelegateHandle);
	FindSessionsDelegateHandle.Reset();
}

void UWeaponMasterGameInstance::JoinSession()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	JoinSessionDelegateHandle = 
		Session->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(
			this,
			&ThisClass::HandleJoinSessionCompleted));

	UE_LOG(LogTemp, Log, TEXT("Joining session."));

	if (!Session->JoinSession(0, "SessionName", *SessionToJoin))
	{
		UE_LOG(LogTemp, Log, TEXT("Join session failed."));

		Session->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionDelegateHandle);
		JoinSessionDelegateHandle.Reset();
	}
}

void UWeaponMasterGameInstance::HandleJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString JoinAddress;
		bool bGotAddress = Session->GetResolvedConnectString(SessionName, JoinAddress);
        
		if (GEngine && bGotAddress)
		{
			UE_LOG(LogTemp, Warning, TEXT("Joined session. JoinAddress: [%s]"), *JoinAddress);
			OnProcessReturnValue.Broadcast(EPlayerEOSStateType::JoinSession, ESessionResultType::Success);
			
			FURL DedicatedServerURL(nullptr, *ConnectString, TRAVEL_Absolute);
			FString DedicatedServerJoinError;
			auto DedicatedServerJoinStatus = GEngine->Browse(GEngine->GetWorldContextFromWorldChecked(GetWorld()), DedicatedServerURL, DedicatedServerJoinError); 
			if (DedicatedServerJoinStatus == EBrowseReturnVal::Failure)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to browse for dedicated server. Error is: %s"), *DedicatedServerJoinError); 
			}
			else if (DedicatedServerJoinStatus == EBrowseReturnVal::Success)
			{
				UE_LOG(LogTemp, Log, TEXT("Joined Success"));
			}

			// To be thorough here you should modify your derived UGameInstance to handle the NetworkError and TravelError events. 
			// This should also be done to handle the error "FULL" returned by the server.
		}
	}

	Session->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionDelegateHandle);
	JoinSessionDelegateHandle.Reset();
}

FString UWeaponMasterGameInstance::GetPlayerName() const
{
	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
	{
		if (IOnlineIdentityPtr Session = Subsystem->GetIdentityInterface())
		{
			if (Session->GetLoginStatus(0) == ELoginStatus::LoggedIn)
			{
				return Session->GetPlayerNickname(0);
			}
		}
	}

	return "";
}

bool UWeaponMasterGameInstance::IsPlayerLoggedIn() const
{
	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
	{
		if (IOnlineIdentityPtr Session = Subsystem->GetIdentityInterface())
		{
			if (Session->GetLoginStatus(0) == ELoginStatus::LoggedIn)
			{
				return true;
			}
		}
	}
	return false;
}
