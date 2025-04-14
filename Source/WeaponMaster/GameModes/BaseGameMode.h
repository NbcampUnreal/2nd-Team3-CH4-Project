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
	virtual void HandlePlayerDeath(TSubclassOf<ACharacter> CharacterClass, APlayerController* Controller) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
};
