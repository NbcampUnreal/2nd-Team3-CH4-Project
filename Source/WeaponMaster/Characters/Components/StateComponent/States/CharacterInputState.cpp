// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterInputState.h"
#include "GameFramework/Character.h"

void UCharacterInputState::Initialize(ACharacter* NewOwnerCharacter)
{
	OwnerCharacter = NewOwnerCharacter;	
}

void UCharacterInputState::Enter()
{
}

void UCharacterInputState::Exit()
{
}

void UCharacterInputState::Tick(float DeltaTime)
{
}

void UCharacterInputState::Move(const FInputActionValue& Value)
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->Move(Value); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::Move : Cast Failed."));
	}
}

void UCharacterInputState::CrouchDrop()
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->CrouchDrop(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::CrouchDrop : Cast Failed."));
	}
}

void UCharacterInputState::StopCrouchDrop()
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->StopCrouchDrop(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::StopCrouchDrop : Cast Failed."));
	}
}

void UCharacterInputState::JumpOrDrop()
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->JumpOrDrop(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::JumpOrDrop : Cast Failed."));
	}
}

void UCharacterInputState::ReleaseJump()
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->ReleaseJump(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::ReleaseJump : Cast Failed."));
	}
}

void UCharacterInputState::Dash()
{
	if (auto CastedCharacter = Cast<ISSTInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->Dash(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::Dash : Cast Failed."));
	}
}

void UCharacterInputState::WeakAttack()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->WeakAttack(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::WeakAttack : Cast Failed."));
	}
}

void UCharacterInputState::StrongAttack()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->StrongAttack(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::StrongAttack : Cast Failed."));
	}
}

void UCharacterInputState::Identity()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->Identity(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::Identity : Cast Failed."));
	}
}

void UCharacterInputState::Defence()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->Defence(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::Defence : Cast Failed."));
	}
}

void UCharacterInputState::PickingItem()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->PickingItem(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::PickingItem : Cast Failed."));
	}
}

void UCharacterInputState::MenuOnOff()
{
	if (auto CastedCharacter = Cast<IBaseBattleInputBindFunctions>(OwnerCharacter))
	{
		CastedCharacter->MenuOnOff(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterInputState::MenuOnOff : Cast Failed."));
	}
}
