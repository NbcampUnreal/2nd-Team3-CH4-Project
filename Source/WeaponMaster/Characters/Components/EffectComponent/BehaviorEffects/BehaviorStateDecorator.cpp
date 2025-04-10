// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorStateDecorator.h"
#include "GameFramework/Character.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/StateComponent/StateComponent.h"
#include "Characters/Components/EffectComponent/EffectComponent.h"

void UBehaviorStateDecorator::Activate()
{
	if (bIsActive)
	{
		return;
	}

	auto OwnerCharacter = Cast<ACharacter>(GetOuter()->GetOuter());
	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogTemp, Display, TEXT("UBehaviorStateDecorator::Activate : Outer Character is Null"));
		return;
	}

	if (OwnerCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
	{
		bIsActive = true;
		SetInnerState(IBattleSystemUser::Execute_GetBehaviorState(GetOuter()->GetOuter()));
		
		Cast<IBehaviorState>(InnerState.GetObject())->SetOuterState(this);
		
		IBattleSystemUser::Execute_SetBehaviorState(GetOuter()->GetOuter(), this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::Activate : Character needs to implement IBattleSystemUser!"));
	}

	if (auto EffectComponent = Cast<UEffectComponent>(GetOuter()))
	{
		EffectComponent->GetActiveBehaviorEffects().Add(BehaviorEffectType);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::Activate : EffectComponent Cast Failed!"));
	}
}

void UBehaviorStateDecorator::Activate(float Duration)
{
	// Active하면 지속시간 갱신
	if (bIsActive)
	{
		GetOuter()->GetWorld()->GetTimerManager().ClearTimer(DurationTimer);
		GetOuter()->GetWorld()->GetTimerManager().SetTimer(
			DurationTimer,
			this,
			&UBehaviorStateDecorator::Deactivate,
			Duration,
			false
		);
		return;
	}
	
	auto OwnerCharacter = Cast<ACharacter>(GetOuter()->GetOuter());
	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogTemp, Display, TEXT("UBehaviorStateDecorator::Activate : Outer Character is Null"));
		return;
	}
	
	if (OwnerCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
	{
		bIsActive = true;
		SetInnerState(IBattleSystemUser::Execute_GetBehaviorState(GetOuter()->GetOuter()));
		
		Cast<IBehaviorState>(InnerState.GetObject())->SetOuterState(this);
		
		IBattleSystemUser::Execute_SetBehaviorState(GetOuter()->GetOuter(), this);
		
		GetOuter()->GetWorld()->GetTimerManager().SetTimer(
			DurationTimer,
			this,
			&UBehaviorStateDecorator::Deactivate,
			Duration,
			false
		);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::Activate : Character needs to implement IBattleSystemUser!"));
	}

	if (auto EffectComponent = Cast<UEffectComponent>(GetOuter()))
	{
		EffectComponent->GetActiveBehaviorEffects().Add(BehaviorEffectType);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::Activate : EffectComponent Cast Failed!"));
	}
}

void UBehaviorStateDecorator::Deactivate()
{
	// InnerState, OuterState 연결하고 여기는 nullptr로
	// 변수들 원래대로
	// GetOuter()->GetWorld()->GetTimerManager().ClearTimer(DurationTimer);
	
	auto OwnerCharacter = Cast<ACharacter>(GetOuter()->GetOuter());
	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogTemp, Display, TEXT("UBehaviorStateDecorator::Deactivate : Outer Character is Null"));
		return;
	}
	
	Cast<IBehaviorState>(InnerState.GetObject())->SetOuterState(OuterState);
	
	if (auto CastedOuterDecorator = Cast<IBehaviorDecorator>(OuterState.GetObject()))
	{
		CastedOuterDecorator->SetInnerState(InnerState);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("UBehaviorStateDecorator::Deactivate : None Outer. Set BehaviorState to Inner."));
		IBattleSystemUser::Execute_SetBehaviorState(OwnerCharacter, InnerState);
	}

	SetInnerState(nullptr);
	SetOuterState(nullptr);

	UE_LOG(LogTemp, Display, TEXT("UBehaviorStateDecorator::Deactivate : Effect End!"));
	bIsActive = false;

	if (auto EffectComponent = Cast<UEffectComponent>(GetOuter()))
	{
		TArray<EBehaviorEffect>& ActiveBehaviorEffects = EffectComponent->GetActiveBehaviorEffects();
		if (ActiveBehaviorEffects.Contains(BehaviorEffectType))
		{
			ActiveBehaviorEffects.Remove(BehaviorEffectType);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UBehaviorStateDecorator::Deactivate : Activated Effect Didn't Save in ActiveBehaviorEffects Array!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::Deactivate : EffectComponent Cast Failed!"));
	}
}

void UBehaviorStateDecorator::Tick(float DeltaTime)
{
}

void UBehaviorStateDecorator::Move(const FInputActionValue& Value)
{
	Cast<IBehaviorState>(InnerState.GetObject())->Move(Value);
}

void UBehaviorStateDecorator::CrouchDrop()
{
	Cast<IBehaviorState>(InnerState.GetObject())->CrouchDrop();
}

void UBehaviorStateDecorator::StopCrouchDrop()
{
	Cast<IBehaviorState>(InnerState.GetObject())->StopCrouchDrop();
}

void UBehaviorStateDecorator::JumpOrDrop()
{
	Cast<IBehaviorState>(InnerState.GetObject())->JumpOrDrop();
}

void UBehaviorStateDecorator::ReleaseJump()
{
	Cast<IBehaviorState>(InnerState.GetObject())->ReleaseJump();
}

void UBehaviorStateDecorator::Dash()
{
	Cast<IBehaviorState>(InnerState.GetObject())->Dash();
}

void UBehaviorStateDecorator::WeakAttack()
{
	Cast<IBehaviorState>(InnerState.GetObject())->WeakAttack();
}

void UBehaviorStateDecorator::StrongAttack()
{
	Cast<IBehaviorState>(InnerState.GetObject())->StrongAttack();
}

void UBehaviorStateDecorator::Identity()
{
	Cast<IBehaviorState>(InnerState.GetObject())->Identity();
}

void UBehaviorStateDecorator::Defence()
{
	Cast<IBehaviorState>(InnerState.GetObject())->Defence();
}

void UBehaviorStateDecorator::PickingItem()
{
	Cast<IBehaviorState>(InnerState.GetObject())->PickingItem();
}

void UBehaviorStateDecorator::MenuOnOff()
{
	Cast<IBehaviorState>(InnerState.GetObject())->MenuOnOff();
}

TScriptInterface<UBehaviorState> UBehaviorStateDecorator::GetInnerState() const
{
	return InnerState;
}

void UBehaviorStateDecorator::SetInnerState(const TScriptInterface<UBehaviorState>& NewInnerState)
{
	InnerState = NewInnerState;
}

TScriptInterface<UBehaviorState> UBehaviorStateDecorator::GetOuterState() const
{ 
	return OuterState;
}

void UBehaviorStateDecorator::SetOuterState(const TScriptInterface<UBehaviorState>& NewOuterState)
{
	OuterState = NewOuterState;
}

