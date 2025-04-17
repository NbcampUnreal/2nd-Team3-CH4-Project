#include "WeaponMasterGameState.h"

#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "PlayerControllers/EOSPlayerController.h"
#include "PlayerControllers/WeaponMasterController.h"
#include "UI/MultiUI/ChatMessageWidget.h"
#include "UI/MultiUI/DeathMatchHUD.h"
#include "UI/MultiUI/IndividualMatchStatusWidget.h"

AWeaponMasterGameState::AWeaponMasterGameState()
{
	bIsPVP = true;
	CooperateVotedPlayerNum = 0;
	DeathMatchVotedPlayerNum = 0;
}

void AWeaponMasterGameState::Multicast_ChatMessage_Implementation(const FText& Message)
{
	for (const APlayerState* PS : PlayerArray)
	{
		if (const AWeaponMasterController* PC = Cast<AWeaponMasterController>(PS->GetOwner()))
		{
			if (const ADeathMatchHUD* DMHUD = Cast<ADeathMatchHUD>(PC->GetHUD()))
			{
				DMHUD->IndividualMatchStatusWidget->AddChatMessage(Message);
			}
		}
	}
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

	DOREPLIFETIME(AWeaponMasterGameState, bIsPVP);
	DOREPLIFETIME(AWeaponMasterGameState, CooperateVotedPlayerNum);
	DOREPLIFETIME(AWeaponMasterGameState, DeathMatchVotedPlayerNum);
}
