#include "WeaponMasterGameState.h"

#include "Net/UnrealNetwork.h"
#include "PlayerControllers/EOSPlayerController.h"

AWeaponMasterGameState::AWeaponMasterGameState()
{
	CooperateVotedPlayerNum = 0;
	DeathMatchVotedPlayerNum = 0;
}

void AWeaponMasterGameState::OnRep_PlayerNumChanged() const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AEOSPlayerController* PC = Cast<AEOSPlayerController>(It->Get()))
		{
			PC->SetSelectedPlayerWidget();
		}
	}
}

void AWeaponMasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponMasterGameState, CooperateVotedPlayerNum);
	DOREPLIFETIME(AWeaponMasterGameState, DeathMatchVotedPlayerNum);
}
