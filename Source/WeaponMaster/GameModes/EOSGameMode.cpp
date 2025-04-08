#include "EOSGameMode.h"

#include "Session/EOSGameSession.h"
#include "WeaponMaster/PlayerControllers/EOSPlayerController.h"

// 세션 로비 게임모드
AEOSGameMode::AEOSGameMode()
{
	PlayerControllerClass = AEOSPlayerController::StaticClass();
	GameSessionClass = AEOSGameSession::StaticClass();
}

void AEOSGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (AEOSGameSession* EOSSession = Cast<AEOSGameSession>(GameSessionClass))
	{
		EOSSession->OnProcessReturnValue.AddUObject(this, &AEOSGameMode::HandleProcessResult);
	}
}

void AEOSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	FSinglePlayerController Entry;
	Entry.Controller = NewPlayer;
	PlayerControllerList.Controller.Add(Entry);
}

APlayerController* AEOSGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	// 로그인 검증 로직
}

void AEOSGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	FUniqueNetIdRepl UniqueNetIdRepl;
	if (TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId())
	{
		if (AEOSGameSession* MyGameSession = GetEOSGameSession())
		{
			if (APlayerController* PC = Cast<APlayerController>(Exiting))
			{
				MyGameSession->RegisterPlayer(PC, UniqueNetId, false);
			}
		}
	}

	if (APlayerController* PC = Cast<APlayerController>(Exiting))
	{
		FSinglePlayerController ExitEntry;
		ExitEntry.Controller = PC;

		if (PlayerControllerList.Controller.Remove(ExitEntry) > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("Removed controller from PlayerControllerList: %s"), *PC->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Controller not found in PlayerControllerList: %s"), *PC->GetName());
		}
	}
}

void AEOSGameMode::HandleProcessResult(EMyStateType State, EMyResultType Result)
{
	UE_LOG(LogTemp, Warning, TEXT("Process State is %d, Result is %d"), State, Result);
	switch (State)
	{
	case EMyStateType::Register:
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Successfully Registered!!"));
			break;
		}
	case EMyStateType::StartSession:
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Started Successfully!!!"));
			TravelMap();
			break;
		}
	default: break;
	}
}

void AEOSGameMode::RegisterPlayer(APlayerController* NewPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("RegisterPlayer"));
	if (NewPlayer)
	{
		FUniqueNetIdRepl UniqueNetIdRepl;
		if (NewPlayer->IsLocalController())
		{
			if (ULocalPlayer *LocalPlayer = NewPlayer->GetLocalPlayer())
			{
				UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
			}
			else
			{
				// 연결된 네트워크 커넥션을 통한 PlayerId 흭득
				UNetConnection *RemoteNetConnection = Cast<UNetConnection>(NewPlayer->Player);
				check(IsValid(RemoteNetConnection));
				UniqueNetIdRepl = RemoteNetConnection->PlayerId;
			}
		}
		else
		{
			// 연결된 네트워크 커넥션을 통한 PlayerId 흭득
			UNetConnection *RemoteNetConnection = Cast<UNetConnection>(NewPlayer->Player);
			check(IsValid(RemoteNetConnection));
			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}

		if (TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId())
		{
			if (AEOSGameSession* MyGameSession = GetEOSGameSession())
			{
				MyGameSession->RegisterPlayer(NewPlayer, UniqueNetId, false);
			}
		}
		
	}
}

void AEOSGameMode::TravelMap()
{
	UE_LOG(LogTemp, Warning, TEXT("TravelMap"));
	GetWorld()->ServerTravel("/Game/WeaponMaster/Maps/Session/PVEMap", true);
	//GetWorld()->ServerTravel("/Game/WeaponMaster/Maps/Session/PVPMap");
}

void AEOSGameMode::StartSession()
{
	if (AEOSGameSession* EOSSession = Cast<AEOSGameSession>(GameSessionClass))
	{
		EOSSession->StartSession();
	}
}


