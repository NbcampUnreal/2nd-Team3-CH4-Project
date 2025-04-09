// Fill out your copyright notice in the Description page of Project Settings.

#include "StunEffect.h"
#include "GameFramework/Character.h"
#include "Characters/Components/IBattleSystemUser.h"

UStunEffect::UStunEffect()
{
	BehaviorEffectType = EBehaviorEffect::Stun;
	bIsActive = false;
}

void UStunEffect::Activate()
{
	if (auto CastedCharacter = Cast<IBattleSystemUser>(OwnerCharacter))
	{
		bIsActive = true;
		InnerState = CastedCharacter->GetBehaviorState();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStunEffect::Activate : Character Cast Failed!"));
	}
}

void UStunEffect::Activate(float Duration)
{
	// 타이머 추가
	// InnerState 세팅
	// IsActive true로
	if (auto CastedCharacter = Cast<IBattleSystemUser>(OwnerCharacter))
	{
		bIsActive = true;
		
		SetInnerState(CastedCharacter->GetBehaviorState());
		InnerState->SetOuterState(this);

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
		UE_LOG(LogTemp, Error, TEXT("UStunEffect::Activate : Character Cast Failed!"));
	}
}

void UStunEffect::Deactivate()
{
	// InnerState, OuterState 연결하고 여기는 nullptr로
	// 변수들 원래대로
	GetOuter()->GetWorld()->GetTimerManager().ClearTimer(DurationTimer);
	
	if (IsValid(OuterState))
	{
		InnerState->SetOuterState(OuterState);
		if (auto CastedOuter = Cast<UBehaviorStateDecorator>(OuterState))
		{
			CastedOuter->SetInnerState(InnerState);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UStunEffect::DeActivate : OuterState Cast Failed!"));
		}
	}
	else
	{
		InnerState->SetOuterState(nullptr);
	}

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

// Not Blocking Menu key
void UStunEffect::MenuOnOff()
{
	InnerState->MenuOnOff();
}