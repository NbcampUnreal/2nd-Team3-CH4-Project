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
	Cast<ISSTInputBindFunctions>(OwnerCharacter)->Move(Value); 
}

void UCharacterInputState::CrouchDrop()
{
	Cast<ISSTInputBindFunctions>(OwnerCharacter)->CrouchDrop(); 
}

void UCharacterInputState::StopCrouchDrop()
{
	Cast<ISSTInputBindFunctions>(OwnerCharacter)->StopCrouchDrop(); 
}

void UCharacterInputState::JumpOrDrop()
{
	Cast<ISSTInputBindFunctions>(OwnerCharacter)->JumpOrDrop(); 
}

void UCharacterInputState::ReleaseJump()
{
	Cast<ISSTInputBindFunctions>(OwnerCharacter)->ReleaseJump(); 
}

void UCharacterInputState::Dash()
{
	Cast<ISSTInputBindFunctions>(OwnerCharacter)->Dash(); 
}

void UCharacterInputState::WeakAttack()
{
	Cast<IBaseBattleInputBindFunctions>(OwnerCharacter)->WeakAttack(); 
}

void UCharacterInputState::StrongAttack()
{
	Cast<IBaseBattleInputBindFunctions>(OwnerCharacter)->StrongAttack(); 
}

void UCharacterInputState::Identity()
{
	Cast<IBaseBattleInputBindFunctions>(OwnerCharacter)->Identity(); 
}

void UCharacterInputState::Defence()
{
	Cast<IBaseBattleInputBindFunctions>(OwnerCharacter)->Defence(); 
}

void UCharacterInputState::PickingItem()
{
	Cast<IBaseBattleInputBindFunctions>(OwnerCharacter)->PickingItem(); 
}

void UCharacterInputState::MenuOnOff()
{
	Cast<IBaseBattleInputBindFunctions>(OwnerCharacter)->MenuOnOff(); 
}
