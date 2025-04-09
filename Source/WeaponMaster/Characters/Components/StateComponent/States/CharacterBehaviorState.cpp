// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBehaviorState.h"
#include "GameFramework/Character.h"

void UCharacterBehaviorState::Initialize(ACharacter* NewOwnerCharacter)
{
	OwnerCharacter = NewOwnerCharacter;	
}

void UCharacterBehaviorState::Enter()
{
}

void UCharacterBehaviorState::Exit()
{
}

void UCharacterBehaviorState::Tick(float DeltaTime)
{
}

void UCharacterBehaviorState::Move(const FInputActionValue& Value)
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
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
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
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
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
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
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
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
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
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
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
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
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
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
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
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
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
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
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
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
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
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
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->MenuOnOff(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterBehaviorState::MenuOnOff : Cast Failed."));
	}
}
