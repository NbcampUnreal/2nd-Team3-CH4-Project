#include "BaseGameMode.h"

#include "EngineUtils.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/ItemComponent/ItemComponent.h"
#include "CharacterSpawner/CharacterSpawner.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/WeaponMasterController.h"
#include "PlayerState/WeaponMasterPlayerState.h"  // Added include for WeaponMasterPlayerState

ABaseGameMode::ABaseGameMode()
{
	
}

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	FString PlayerName = "Player";
	if (const auto CastedGameInstance = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
	{
		PlayerName = CastedGameInstance->GetPlayerName();
		
		if (PlayerName.IsEmpty())
		{
			PlayerName = FString::Printf(TEXT("Player_%d"), NewPlayer->GetUniqueID());
		}
		
		// Set player name in PlayerState
		if (AWeaponMasterPlayerState* PlayerState = Cast<AWeaponMasterPlayerState>(NewPlayer->PlayerState))
		{
			PlayerState->SetPlayerName(PlayerName);
			UE_LOG(LogTemp, Display, TEXT("Player name set from GameInstance: %s"), *PlayerName);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("ABaseGameMode::PostLogin : PlayerState Cast Failed"));
		}
		
		
		TSubclassOf<ACharacter> CharacterClass = CastedGameInstance->CharacterClass;
		FName ItemName = CastedGameInstance->ItemName;
	
		SpawnPlayerCharacter(NewPlayer);

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (AEOSPlayerController* PlayerController = Cast<AEOSPlayerController>(Iterator->Get()))
			{
				PlayerController->Client_UpdatePlayers();
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABaseGameMode::PostLogin : GameInstance Cast Failed."))
	}
}

bool ABaseGameMode::HasCharacterSpawner() const
{
	for (TActorIterator<ACharacterSpawner> It(GetWorld()); It; ++It)
	{
		return true; 
	}
	return false;
}

void ABaseGameMode::SpawnPlayerCharacter(APlayerController* Controller)
{
	// 하위 게임모드에서 구현
}

void ABaseGameMode::HandlePlayerDeath(APlayerController* Controller, const FString& AttackerName)
{
	// 게임모드에 특정 함수 호출
	// 타이머 + 흑백UI
	SpawnPlayerCharacter(Controller);

	// Kill Log
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (AEOSPlayerController* PlayerController = Cast<AEOSPlayerController>(Iterator->Get()))
		{
			UE_LOG(LogTemp, Warning, TEXT("ABaseGameMode::HandlePlayerDeath"))
			PlayerController->Client_PlayerDead(GetPlayerNameFromGameInstance(Controller), AttackerName);
		}
	}
}

// Helper function to get player name from GameInstance
FString ABaseGameMode::GetPlayerNameFromGameInstance(APlayerController* Controller)
{
	if (Controller == nullptr)
	{
		return TEXT("Unknown");
	}

	if (const UWeaponMasterGameInstance* GameInstance = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
	{
		FString PlayerName = GameInstance->GetPlayerName();
		if (!PlayerName.IsEmpty())
		{
			return PlayerName;
		}
	}

	// Fallback to PlayerState name if available
	if (Controller->PlayerState)
	{
		return Controller->PlayerState->GetPlayerName();
	}
	
	return FString::Printf(TEXT("Player_%d"), Controller->GetUniqueID());
}