#pragma once

#include "CoreMinimal.h"
#include "BattleGMInterface.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameMode.generated.h"

UCLASS()
class WEAPONMASTER_API ABaseGameMode : public AGameModeBase, public IBattleGMInterface
{
	GENERATED_BODY()

public:
	ABaseGameMode();

	virtual void BeginPlay() override;
	virtual bool HasCharacterSpawner() const;
	
	// BattleGMInterface
	virtual void SpawnPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, APlayerController* Controller) override;
	virtual void HandlePlayerDeath(const TSubclassOf<ACharacter>& CharacterClass, APlayerController* Controller, const FString& AttackerName) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	// Helper function to get player name from GameInstance
	UFUNCTION(BlueprintCallable, Category = "Player")
	FString GetPlayerNameFromGameInstance(APlayerController* Controller);
};