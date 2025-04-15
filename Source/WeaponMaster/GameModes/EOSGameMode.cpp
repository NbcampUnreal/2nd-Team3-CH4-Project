#include "EOSGameMode.h"

#include "GLSMacroses.h"
#include "GameState/WeaponMasterGameState.h"
#include "PlayerState/WeaponMasterPlayerState.h"
#include "Session/EOSGameSession.h"
#include "WeaponMaster/PlayerControllers/EOSPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(GLS_LOG_CATEGORY, Log, All);
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
		EOSSession->OnSessionReturnValue.AddUObject(this, &AEOSGameMode::HandleProcessResult);
	}
}

void AEOSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	TimerCountDown = FMath::Clamp(TimerCountDown + 20, 0, TimerCountDown);
}

void AEOSGameMode::SetTimer()
{
	if (IsRunningDedicatedServer())
	{
		GetWorldTimerManager().SetTimer(
		PlayCountDownTimerHandle,
		this,
		&AEOSGameMode::PlayCountDownTimerAction,
		1.0f,
		true
		);
	}
}

void AEOSGameMode::PlayCountDownTimerAction()
{
	--TimerCountDown;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (AEOSPlayerController* PlayerController = Cast<AEOSPlayerController>(Iterator->Get()))
		{
			PlayerController->Client_UpdateTimer(TimerCountDown);
		}
	}

	if (TimerCountDown == 0)
	{
		StartSession();
		
		if (const AWeaponMasterGameState* WeaponMasterGameState = Cast<AWeaponMasterGameState>(GameState))
		{
			if (WeaponMasterGameState->IsVotedEqual())
			{
				// 강제로 랜덤 맵으로 이동 후 시작
				MapURL = "/Game/WeaponMaster/Maps/Session/PVPMap";
			}
			else
			{
				if (WeaponMasterGameState->IsCooperateVotedMore())
				{
					// 협동맵으로 이동
					MapURL = "/Game/WeaponMaster/Maps/Session/PVEMap";
				}
				else
				{
					// 대전맵으로 이동
					MapURL = "/Game/WeaponMaster/Maps/Session/PVPMap";
				}
			}
		}
		
		TravelMap();
		GetWorldTimerManager().ClearTimer(PlayCountDownTimerHandle);
	}
}

APlayerController* AEOSGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	SetTimer();
	
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void AEOSGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	FUniqueNetIdRepl UniqueNetIdRepl;
	if (TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId())
	{
		if (AEOSGameSession* MyGameSession = GetEOSGameSession())
		{
			if (const APlayerController* PC = Cast<APlayerController>(Exiting))
			{
				MyGameSession->UnregisterPlayer(PC);
			}
		}
	}
}

void AEOSGameMode::HandleProcessResult(ESessionStateType State, ESessionResultType Result)
{
	UE_LOG(LogTemp, Warning, TEXT("Process State is %d, Result is %d"), State, Result);
	switch (State)
	{
	case ESessionStateType::Register:
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Successfully Registered!!"));
			break;
		}
	case ESessionStateType::StartSession:
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Started Successfully!!!"));
			TravelMap();
			break;
		}
	case ESessionStateType::PlayerNumChanged:
		{
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				if (AEOSPlayerController* PlayerController = Cast<AEOSPlayerController>(Iterator->Get()))
				{
					PlayerController->Client_UpdateTotalPlayerNum(GetPlayerNum());
				}
			}
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
			AWeaponMasterPlayerState* PlayerState = NewPlayer->GetPlayerState<AWeaponMasterPlayerState>();
			PlayerState->SetUniqueId(UniqueNetId);
			
			if (AEOSGameSession* MyGameSession = GetEOSGameSession())
			{
				MyGameSession->RegisterPlayer(NewPlayer, UniqueNetId, false);
			}
		}
	}
}

void AEOSGameMode::TravelMap()
{
	UE_LOG(LogTemp, Warning, TEXT("TravelMap To [%s]"), *MapURL);
	if (MapURL == "") return;
	GetWorld()->ServerTravel(MapURL, true);
}

void AEOSGameMode::StartSession()
{
	UE_LOG(LogTemp, Warning, TEXT("AEOSGameMode::StartSession"));
	if (AEOSGameSession* EOSSession = GetEOSGameSession())
	{
		EOSSession->StartSession();
	}
}

int32 AEOSGameMode::GetPlayerNum() const
{
	if (AEOSGameSession* EOSSession = GetEOSGameSession())
	{
		return EOSSession->NumberOfPlayersInSession;
	}

	return 0;
}


