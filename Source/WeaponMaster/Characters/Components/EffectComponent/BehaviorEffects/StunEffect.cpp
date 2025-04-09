// Fill out your copyright notice in the Description page of Project Settings.

#include "StunEffect.h"
#include "GameFramework/Character.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/StateComponent/StateComponent.h"

UStunEffect::UStunEffect()
{
	BehaviorEffectType = EBehaviorEffect::Stun;
	bIsActive = false;
}
 
void UStunEffect::Activate()
{
	if (GetOuter()->GetOuter()->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
	{
		bIsActive = true;
		SetInnerState(IBattleSystemUser::Execute_GetBehaviorState(GetOuter()->GetOuter()));
		
		Cast<IBehaviorState>(InnerState.GetObject())->SetOuterState(this);

		IBattleSystemUser::Execute_SetBehaviorState(GetOuter()->GetOuter(), this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStunEffect::Activate : Character Cast Failed!"));
	}
}

void UStunEffect::Activate(float Duration)
{
	// Active하면 지속시간 갱신
	if (bIsActive)
	{
		GetOuter()->GetWorld()->GetTimerManager().ClearTimer(DurationTimer);
		GetOuter()->GetWorld()->GetTimerManager().SetTimer(
			DurationTimer,
			this,
			&UStunEffect::Deactivate,
			Duration,
			false
		);
		return;
	}
	
	auto OwnerCharacter = Cast<ACharacter>(GetOuter()->GetOuter());
	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogTemp, Display, TEXT("UStunEffect::Activate : Outer Character is Null"));
		return;
	}
	
	if (OwnerCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
	{
		bIsActive = true;
		SetInnerState(IBattleSystemUser::Execute_GetBehaviorState(GetOuter()->GetOuter()));
		
		// Cast<IBehaviorState>(InnerState.GetObject())->SetOuterState(TScriptInterface<UBehaviorState>(this));
		Cast<IBehaviorState>(InnerState.GetObject())->SetOuterState(this);
		
		// IBattleSystemUser::Execute_SetBehaviorState(GetOuter()->GetOuter(), TScriptInterface<UBehaviorState>(this));
		IBattleSystemUser::Execute_SetBehaviorState(GetOuter()->GetOuter(), this);
		
		GetOuter()->GetWorld()->GetTimerManager().SetTimer(
			DurationTimer,
			this,
			&UStunEffect::Deactivate,
			Duration,
			false
		);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStunEffect::Activate : Character needs to implement IBattleSystemUser!"));
	}
}

void UStunEffect::Deactivate()
{
	// InnerState, OuterState 연결하고 여기는 nullptr로
	// 변수들 원래대로
	// GetOuter()->GetWorld()->GetTimerManager().ClearTimer(DurationTimer);
	
	Cast<IBehaviorState>(InnerState.GetObject())->SetOuterState(OuterState);
	
	if (auto CastedOuterDecorator = Cast<IBehaviorDecorator>(OuterState.GetObject()))
	{
		CastedOuterDecorator->SetInnerState(InnerState);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("UStunEffect::Deactivate : None Outer. Set BehaviorState to Inner."));
		// IBattleSystemUser::Execute_SetBehaviorState(GetOuter()->GetOuter(), InnerState.GetObject());
		IBattleSystemUser::Execute_SetBehaviorState(GetOuter()->GetOuter(), InnerState);
	}

	SetInnerState(nullptr);
	SetOuterState(nullptr);

	UE_LOG(LogTemp, Display, TEXT("UStunEffect::DeActivate : Stun End!"));
	bIsActive = false;
}

void UStunEffect::Move(const FInputActionValue& Value)
{
}

void UStunEffect::CrouchDrop()
{
}

void UStunEffect::StopCrouchDrop()
{
}

void UStunEffect::JumpOrDrop()
{
}

void UStunEffect::ReleaseJump()
{
}

void UStunEffect::Dash()
{
}

void UStunEffect::WeakAttack()
{
}

void UStunEffect::StrongAttack()
{
}

void UStunEffect::Identity()
{
}

void UStunEffect::Defence()
{
}

void UStunEffect::PickingItem()
{
}

// Outer3 -> Outer2 -> Outer1 -> Inner0
//
// Outer3->fff()
// fff
// {
// 	Suepr::fff;
// 	Inner->fff();
// )


// // Not Blocking Menu key
// void UStunEffect::MenuOnOff()
// {
// 	InnerState->MenuOnOff();
// }
//
// ACharacter* UStunEffect::GetOwnerCharacter() const
// {
// 	return InnerState->GetOwnerCharacter();
// }
