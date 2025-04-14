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

	// Get player name from GameInstance
	FString PlayerName = "Player";
	if (const auto CastedGameInstance = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
	{
		PlayerName = CastedGameInstance->GetPlayerName();
		
		// If no valid name found from EOS, use a default with player index
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
		
		TSubclassOf<ACharacter> CharacterClass = CastedGameInstance->CharacterClass;
		FName ItemName = CastedGameInstance->ItemName;
	
		SpawnPlayerCharacter(CharacterClass, NewPlayer);
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

void ABaseGameMode::SpawnPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, APlayerController* Controller)
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseGameMode::SpawnPlayerCharacter"))
	if (!HasCharacterSpawner()) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterSpawner::StaticClass(), FoundActors);

	uint8 Cnt = 0;

	while (true)
	{
		uint8 RandomSpawnerIndex = FMath::RandRange(0, FoundActors.Num() - 1);
		ACharacterSpawner* CharacterSpawner = Cast<ACharacterSpawner>(FoundActors[RandomSpawnerIndex]);

		if (ACharacter* SpawnCharacter = CharacterSpawner->SpawnCharacter(CharacterClass))
		{
			if (AWeaponMasterController* WMPC = Cast<AWeaponMasterController>(Controller))
			{
				SpawnCharacter->SetOwner(WMPC);
				WMPC->Possess(SpawnCharacter);
				UE_LOG(LogTemp, Warning, TEXT("Possessed Pawn: %s"), *WMPC->GetPawn()->GetName());
			}

			if (const UWeaponMasterGameInstance* WMGI = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
			{
				if (SpawnCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
				{
					UItemComponent* ItemComponent = IBattleSystemUser::Execute_GetItemComponent(SpawnCharacter);
					ItemComponent->EquipItem(WMGI->ItemName);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SetPlayerCharacter : SpawnCharacter doesn't implement IBattleSystemUser."));
				}
			}
			break;
		}

		// Place CharacterSpawners more than PlayerNumbers.
		// You need to consider character size not to overlap when character spawned.
		check(++Cnt < 200);
	}
}

void ABaseGameMode::HandlePlayerDeath(const TSubclassOf<ACharacter> CharacterClass, APlayerController* Controller)
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseGameMode::HandlePlayerDeath"))
	// Kill Log

	// 게임모드에 특정 함수 호출
	SpawnPlayerCharacter(CharacterClass, Controller);
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