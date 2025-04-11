#include "WeaponMasterGameState.h"

#include "Net/UnrealNetwork.h"

AWeaponMasterGameState::AWeaponMasterGameState()
{
	CooperateVotedPlayerNum = 0;
	DeathMatchVotedPlayerNum = 0;
}

void AWeaponMasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponMasterGameState, CooperateVotedPlayerNum);
	DOREPLIFETIME(AWeaponMasterGameState, DeathMatchVotedPlayerNum);
}
