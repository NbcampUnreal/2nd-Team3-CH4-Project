#pragma once

#include "CoreMinimal.h"
#include "BattleGMInterface.h"
#include "GameFramework/GameModeBase.h"
#include "SingleGameMode.generated.h"

UCLASS()
class WEAPONMASTER_API ASingleGameMode : public AGameModeBase, public IBattleGMInterface
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay()override;

	void NotifyCharacterDeath(AActor* DeadActor);
	void OnPlayerWin();
	void OnPlayerLose();
	
	virtual void SetPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, FName ItemName, AController* OwnerController) override;

private:
	bool HasCharacterSpawner() const;
};