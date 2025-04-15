#include "GameModes/TeamGameMode.h"
#include "PlayerControllers/EOSPlayerController.h"
#include "PlayerState/WeaponMasterPlayerState.h"
#include "UI/MultiUI/GameResultWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"

void ATeamGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay called in ATeamGameMode"));
	SetTimer();
}

void ATeamGameMode::SetTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("SetTimer called"));

	GetWorldTimerManager().SetTimer(
		PlayCountDownTimerHandle,
		this,
		&ATeamGameMode::PlayCountDownTimerAction,
		1.0f,
		true
	);
}

void ATeamGameMode::PlayCountDownTimerAction()
{
	UE_LOG(LogTemp, Warning, TEXT("Timer Tick: %d"), TimerCountDown);

	if (TimerCountDown > 0 || TimerCountDown != 0)
	{
		--TimerCountDown;
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (AEOSPlayerController* PlayerController = Cast<AEOSPlayerController>(Iterator->Get()))
		{
			PlayerController->Client_UpdateInGameTimer(TimerCountDown);
		}
	}

	if (TimerCountDown == 0)
	{
		// Score Board 이동
		UE_LOG(LogTemp, Warning, TEXT("Timer ended. Calling BroadcastGameResultsToClients()"));

		GetWorldTimerManager().ClearTimer(PlayCountDownTimerHandle);

		BroadcastGameResultsToClients(999);
	}
}

void ATeamGameMode::BroadcastGameResultsToClients(int32 Results)
{
	TArray<FPlayerResultData> ResultList;

	if (!GameState)	return;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		AWeaponMasterPlayerState* WMPS = Cast<AWeaponMasterPlayerState>(PS);
		if (!WMPS)
		{
			UE_LOG(LogTemp, Error, TEXT("Cast to AWeaponMasterPlayerState 실패"));
			continue;
		}

		const FString Name = WMPS->GetPlayerName();
		const int32 Kills = WMPS->GetKillCount();
		const int32 Deaths = WMPS->GetDeathCount();
		const float Damage = WMPS->GetTotalDamageDealt();
		UE_LOG(LogTemp, Warning, TEXT("플레이어 정보 수집됨: %s / K: %d / D: %d / DMG: %.2f"),
			*Name, Kills, Deaths, Damage);

		FPlayerResultData Data;

		Data.Nickname = Name;
		Data.Kills = Results;
		Data.Deaths = Deaths;
		Data.Damage = FMath::RoundToInt(Damage);

		UWeaponMasterGameInstance* GI = WMPS->GetGameInstance<UWeaponMasterGameInstance>();

		if (GI && GI->CharacterClass)
		{
			const ABaseBattleCharacter* DefaultChar = GI->CharacterClass->GetDefaultObject<ABaseBattleCharacter>();
			if (DefaultChar)
			{
				UTexture2D* LoadedTexture = DefaultChar->GetCharacterThumbnail();
				if (LoadedTexture)
				{
					Data.Icon = LoadedTexture;
				}
			}

		}

		ResultList.Add(Data);
	}

	if (ResultList.Num() == 0)return;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AEOSPlayerController* PC = Cast<AEOSPlayerController>(It->Get()))
		{
			if (!IsValid(PC))
			{
				UE_LOG(LogTemp, Error, TEXT("PlayerController가 유효하지 않음"));
				continue;
			}

			if (!PC->GameResultWidgetClass)
			{
				UE_LOG(LogTemp, Error, TEXT("%s: GameResultWidgetClass가 설정되지 않음"), *PC->GetName());
				continue;
			}

			UE_LOG(LogTemp, Warning, TEXT("결과 전송 대상: %s"), *PC->GetName());
			PC->Client_ShowGameResult(ResultList);
		}
	}
}


void ATeamGameMode::OnBossDefeated()
{
	GetWorldTimerManager().ClearTimer(PlayCountDownTimerHandle);
	BroadcastGameResultsToClients(1000);
}
