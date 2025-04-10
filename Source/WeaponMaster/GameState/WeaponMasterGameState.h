#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "WeaponMasterGameState.generated.h"

UCLASS()
class WEAPONMASTER_API AWeaponMasterGameState : public AGameState
{
	GENERATED_BODY()

public:
	AWeaponMasterGameState();
	
	UPROPERTY(Replicated)
	int32 CooperateVotedPlayerNum;

	UPROPERTY(Replicated)
	int32 DeathMatchVotedPlayerNum;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
