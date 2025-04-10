// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBehaviorState.h"
#include "GameFramework/Character.h"
#include "ISSTInputBindFunctions.h"
#include "Characters/BaseBattleCharacter/IBaseBattleInputBindFunctions.h"

void UCharacterBehaviorState::Move(const FInputActionValue& Value)
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->Move(Value); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::Move : Cast Failed."));
	}
}

void UCharacterBehaviorState::CrouchDrop()
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->CrouchDrop(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::CrouchDrop : Cast Failed."));
	}
}

void UCharacterBehaviorState::StopCrouchDrop()
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->StopCrouchDrop(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::StopCrouchDrop : Cast Failed."));
	}
}

void UCharacterBehaviorState::JumpOrDrop()
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->JumpOrDrop(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::JumpOrDrop : Cast Failed."));
	}
}

void UCharacterBehaviorState::ReleaseJump()
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->ReleaseJump(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::ReleaseJump : Cast Failed."));
	}
}

void UCharacterBehaviorState::Dash()
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->Dash(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::Dash : Cast Failed."));
	}
}

void UCharacterBehaviorState::WeakAttack()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->WeakAttack(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::WeakAttack : Cast Failed."));
	}
}

void UCharacterBehaviorState::StrongAttack()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->StrongAttack(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::StrongAttack : Cast Failed."));
	}
}

void UCharacterBehaviorState::Identity()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->Identity(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::Identity : Cast Failed."));
	}
}

void UCharacterBehaviorState::Defence()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->Defence(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::Defence : Cast Failed."));
	}
}

void UCharacterBehaviorState::PickingItem()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->PickingItem(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::PickingItem : Cast Failed."));
	}
}

void UCharacterBehaviorState::MenuOnOff()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(GetOuter()->GetOuter()))
	{
		CastedCharacter->MenuOnOff(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::MenuOnOff : Cast Failed."));
	}
} 

TScriptInterface<UBehaviorState> UCharacterBehaviorState::GetOuterState() const
{
	return OuterState; 
}

void UCharacterBehaviorState::SetOuterState(const TScriptInterface<UBehaviorState>& NewOuterState)
{
	OuterState = NewOuterState;
}
