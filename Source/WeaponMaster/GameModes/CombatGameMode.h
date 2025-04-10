#pragma once

#include "CoreMinimal.h"
#include "BattleGMInterface.h"
#include "GameFramework/GameMode.h"
#include "CombatGameMode.generated.h"

UCLASS()
class WEAPONMASTER_API ACombatGameMode : public AGameMode, public IBattleGMInterface
{
	GENERATED_BODY()
	
public:
	virtual void SetPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, FName ItemName, AController* OwnerController) override;
	
	
};
