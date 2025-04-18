#include "GameModes/TeamGameMode.h"

#include "Characters/Components/IBattleSystemUser.h"
#include "CharacterSpawner/CharacterSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/EOSPlayerController.h"
#include "PlayerControllers/WeaponMasterController.h"
#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "Characters/Components/ItemComponent/ItemComponent.h"
#include "PlayerState/WeaponMasterPlayerState.h"
#include "UI/MultiUI/GameResultWidget.h"
#include "GameFramework/GameStateBase.h"
// #include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "Engine/World.h"
#include "Session/EOSGameSession.h"

void ATeamGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay called in ATeamGameMode"));
	SetTimer();
}

void ATeamGameMode::SpawnPlayerCharacter(APlayerController* Controller)
{
	Super::SpawnPlayerCharacter(Controller);

	UE_LOG(LogTemp, Warning, TEXT("ATeamGameMode::SpawnPlayerCharacter : Called."));
	
	if (!HasCharacterSpawner()) return;
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterSpawner::StaticClass(), FoundActors);

	uint32 Cnt = 50000;

	if (CharacterClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SpawnPlayerCharacter : CharacterClasses Isn't Set"));
		return;
	}

	if (ItemNames.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SpawnPlayerCharacter : ItemNames Isn't Set"));
		return;
	}
	
	auto CharacterClassRandomIndex = FMath::RandRange(0, CharacterClasses.Num() - 1);
	auto ItemNameRandomIndex = FMath::RandRange(0, ItemNames.Num() - 1);

	Controller->GetPlayerState<AWeaponMasterPlayerState>()->CharacterClass = CharacterClasses[CharacterClassRandomIndex];
	
	while (--Cnt)
	{
		uint8 RandomSpawnerIndex = FMath::RandRange(0, FoundActors.Num() - 1);
		ACharacterSpawner* CharacterSpawner = Cast<ACharacterSpawner>(FoundActors[RandomSpawnerIndex]);

		if (ACharacter* SpawnCharacter = CharacterSpawner->SpawnCharacter(CharacterClasses[CharacterClassRandomIndex]))
		{
			if (AWeaponMasterController* WMPC = Cast<AWeaponMasterController>(Controller))
			{
				// Possess할때 Owner설정도 됨!
				// SpawnCharacter->SetOwner(WMPC);
				WMPC->Possess(SpawnCharacter);
				WMPC->SetCurrentCharacterAtGI(CharacterClasses[CharacterClassRandomIndex]);
				// UE_LOG(LogTemp, Display, TEXT("ATeamGameMode::SpawnPlayerCharacter : Chosen Character : %s"), *CharacterClasses[CharacterClassRandomIndex]->GetDisplayNameText().ToString());
				UE_LOG(LogTemp, Display, TEXT("ATeamGameMode::SpawnPlayerCharacter : Spawn And Possessed."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SetPlayerCharacter : Possess Failed."));
			}
			
			UE_LOG(LogTemp, Display, TEXT("ATeamGameMode::SpawnPlayerCharacter : ItemNameRandomIndex : %d"), ItemNameRandomIndex);
			
			if (SpawnCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
			{
				UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SpawnCharacter : Execute_GetItemComponent"));
				UItemComponent* ItemComponent = IBattleSystemUser::Execute_GetItemComponent(SpawnCharacter);
				// UE_LOG(LogTemp, Display, TEXT("ATeamGameMode::SpawnPlayerCharacter : Chosen Item : %s"), *ItemNames[ItemNameRandomIndex].ToString())
				UE_LOG(LogTemp, Display, TEXT("ATeamGameMode::SpawnPlayerCharacter : Item Selected"));
				ItemComponent->EquipItem(ItemNames[ItemNameRandomIndex]);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SetPlayerCharacter : SpawnCharacter doesn't implement IBattleSystemUser."));
			}
			
			break;
		}
	}
}

void ATeamGameMode::SetTimer()
{
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
	if (TimerCountDown > 0)
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
	FString CurrentMapName = GetWorld()->GetMapName();
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
		Data.Deaths = Deaths;
		Data.Damage = FMath::RoundToInt(Damage);
		if (CurrentMapName.Contains(TEXT("PVEMap")))
		{
			Data.Kills = Results;
		}
		else
		{
			Data.Kills = Kills;
		}

		if (const ABaseBattleCharacter* DefaultChar = WMPS->CharacterClass->GetDefaultObject<ABaseBattleCharacter>())
		{
			Data.Icon = DefaultChar->GetCharacterThumbnail();
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("ATeamGameMode::BroadcastGameResultsToClients : Accessing DefaultCharacter Failed."))
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
				continue;
			}
			if (!PC->GameResultWidgetClass)
			{
				continue;
			}

			PC->Client_ShowGameResult(ResultList);

			if (AEOSGameSession* EOSGameSession = Cast<AEOSGameSession>(GameSession))
			{
				EOSGameSession->UnregisterPlayer(PC);
			}
		}
	}

	if (AEOSGameSession* EOSGameSession = Cast<AEOSGameSession>(GameSession))
	{
		EOSGameSession->EndSession();
	}

	GetWorldTimerManager().SetTimer(
		DestorySessionTimerHandle,
		this,
		&ATeamGameMode::DestroySession,
		10.0f,
		false
	);
}

void ATeamGameMode::OnBossDefeated()
{
	GetWorldTimerManager().ClearTimer(PlayCountDownTimerHandle);
	BroadcastGameResultsToClients(1000);
}

void ATeamGameMode::DestroySession()
{
	if (AEOSGameSession* EOSGameSession = Cast<AEOSGameSession>(GameSession))
	{
		EOSGameSession->DestroySession();
	}
}
