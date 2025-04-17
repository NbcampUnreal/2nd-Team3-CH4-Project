// Fill out your copyright notice in the Description page of Project Settings.

#include "UsingSkillEffect.h"
#include "Characters/Components/StateComponent/BufferedInput/BufferedWeakAttack.h"
#include "Characters/Components/StateComponent/BufferedInput/BufferedStrongAttack.h"
#include "Characters/Components/StateComponent/BufferedInput/BufferedIdentity.h"


UUsingSkillEffect::UUsingSkillEffect()
{
	BehaviorEffectType = EBehaviorEffect::UsingSkill;
	bIsActive = false;
	OuterState = nullptr;
	InnerState = nullptr;
}

void UUsingSkillEffect::Activate()
{
	Super::Activate();
}

void UUsingSkillEffect::Activate(float Duration)
{
	Super::Activate(Duration);
}

void UUsingSkillEffect::Deactivate()
{
	Super::Deactivate();
	auto OwnerCharacter = IsValid(GetOuter()) ? Cast<ACharacter>(GetOuter()->GetOuter()) : nullptr;
	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogTemp, Display, TEXT("UUsingSkillEffect::Deactivate : Outer Character is Null"));
		return;
	}
	
	if (OwnerCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
	{
		UStateComponent* StateComponent = IBattleSystemUser::Execute_GetStateComponent(OwnerCharacter);
		TQueue<TUniquePtr<FBaseBufferedInput>>& BufferedInputQueue = StateComponent->BufferedInputQueue;
	
		TUniquePtr<FBaseBufferedInput> BufferedInput;
		while (BufferedInputQueue.Dequeue(BufferedInput))
		{
			if (BufferedInput->IsValidInput())
			{
				BufferedInput->Operate();
				break;
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Buffered Input Expired"));
			}
		}
	}
}

void UUsingSkillEffect::Move(const FInputActionValue& Value)
{
	// Super::Move(Value);
}

void UUsingSkillEffect::CrouchDrop()
{
	// Super::CrouchDrop();
}

void UUsingSkillEffect::StopCrouchDrop()
{
	// Super::StopCrouchDrop();
}

void UUsingSkillEffect::JumpOrDrop()
{
	// Super::JumpOrDrop();
}

void UUsingSkillEffect::ReleaseJump()
{
	// Super::ReleaseJump();
}

void UUsingSkillEffect::Dash()
{
	// Super::Dash();
}

void UUsingSkillEffect::WeakAttack()
{
	// Super::WeakAttack();
	auto OwnerCharacter = IsValid(GetOuter()) ? Cast<ACharacter>(GetOuter()->GetOuter()) : nullptr;
	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogTemp, Display, TEXT("UUsingSkillEffect::WeakAttack : Outer Character is Null"));
		return;
	}
	
	if (OwnerCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
	{
		UStateComponent* StateComponent = IBattleSystemUser::Execute_GetStateComponent(OwnerCharacter);
		TQueue<TUniquePtr<FBaseBufferedInput>>& BufferedInputQueue = StateComponent->BufferedInputQueue;
	
		auto BufferedWeakAttack = MakeUnique<FBufferedWeakAttack>();
		BufferedWeakAttack->Initialize(OwnerCharacter);
		BufferedInputQueue.Enqueue(std::move(BufferedWeakAttack));
		UE_LOG(LogTemp, Display, TEXT("EnQueue"));
	}
}

void UUsingSkillEffect::StrongAttack()
{
	// Super::StrongAttack();
	auto OwnerCharacter = IsValid(GetOuter()) ? Cast<ACharacter>(GetOuter()->GetOuter()) : nullptr;
	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogTemp, Display, TEXT("UUsingSkillEffect::StrongAttack : Outer Character is Null"));
		return;
	}
	
	if (OwnerCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
	{
		UStateComponent* StateComponent = IBattleSystemUser::Execute_GetStateComponent(OwnerCharacter);
		TQueue<TUniquePtr<FBaseBufferedInput>>& BufferedInputQueue = StateComponent->BufferedInputQueue;
	
		auto BufferedStrongAttack = MakeUnique<FBufferedStrongAttack>();
		BufferedStrongAttack->Initialize(OwnerCharacter);
		BufferedInputQueue.Enqueue(std::move(BufferedStrongAttack));
	}
}

void UUsingSkillEffect::Identity()
{
	// Super::Identity();
	auto OwnerCharacter = IsValid(GetOuter()) ? Cast<ACharacter>(GetOuter()->GetOuter()) : nullptr;
	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogTemp, Display, TEXT("UUsingSkillEffect::Identity : Outer Character is Null"));
		return;
	}
	
	if (OwnerCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
	{
		UStateComponent* StateComponent = IBattleSystemUser::Execute_GetStateComponent(OwnerCharacter);
		TQueue<TUniquePtr<FBaseBufferedInput>>& BufferedInputQueue = StateComponent->BufferedInputQueue;
	
		auto BufferedIdentity = MakeUnique<FBufferedIdentity>();
		BufferedIdentity->Initialize(OwnerCharacter);
		BufferedInputQueue.Enqueue(std::move(BufferedIdentity));
	}
}

void UUsingSkillEffect::Defence()
{
	// Super::Defence();
}

void UUsingSkillEffect::PickingItem()
{
	// Super::PickingItem();
}
