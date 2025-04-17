// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorStateDecorator.h"
#include "GameFramework/Character.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/StateComponent/StateComponent.h"
#include "Characters/Components/EffectComponent/EffectComponent.h"
#include "Characters/Components/DebuffWidgetUser.h"
#include "UI/CommonUI/DebuffWidget.h"

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
		SetInnerState(IBattleSystemUser::Execute_GetBehaviorState(OwnerCharacter));
		
		Cast<IBehaviorState>(InnerState.GetObject())->SetOuterState(this);
		
		IBattleSystemUser::Execute_SetBehaviorState(OwnerCharacter, this);
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
		SetInnerState(IBattleSystemUser::Execute_GetBehaviorState(OwnerCharacter));
		
		Cast<IBehaviorState>(InnerState.GetObject())->SetOuterState(this);
		
		IBattleSystemUser::Execute_SetBehaviorState(OwnerCharacter, this);
		
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

	if (auto DebuffWidgetUser = Cast<IDebuffWidgetUser>(OwnerCharacter))
	{
		DebuffWidgetUser->UpdateDebuffWidget();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::Activate :DebuffWidgetUser Cast Failed!"));
	}
}

void UBehaviorStateDecorator::Deactivate()
{
	// InnerState, OuterState 연결하고 여기는 nullptr로
	// 변수들 원래대로
	// GetOuter()->GetWorld()->GetTimerManager().ClearTimer(DurationTimer);
	if (!bIsActive)
	{
		return;
	}
	
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

	if (auto DebuffWidgetUser = Cast<IDebuffWidgetUser>(OwnerCharacter))
	{
		DebuffWidgetUser->UpdateDebuffWidget();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::Deactivate :DebuffWidgetUser Cast Failed!"));
	}
}

void UBehaviorStateDecorator::Tick(float DeltaTime)
{
}

void UBehaviorStateDecorator::Move(const FInputActionValue& Value)
{
	if (IsValid(InnerState.GetObject()))
	{
		Cast<IBehaviorState>(InnerState.GetObject())->Move(Value);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::Move : InnerState Invalid"));
	}
}

void UBehaviorStateDecorator::CrouchDrop()
{
	if (IsValid(InnerState.GetObject()))
	{
		Cast<IBehaviorState>(InnerState.GetObject())->CrouchDrop();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::CrouchDrop : InnerState Invalid"));
	}
}

void UBehaviorStateDecorator::StopCrouchDrop()
{
	if (IsValid(InnerState.GetObject()))
	{
		Cast<IBehaviorState>(InnerState.GetObject())->StopCrouchDrop();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::StopCrouchDrop : InnerState Invalid"));
	}
}

void UBehaviorStateDecorator::JumpOrDrop()
{
	if (IsValid(InnerState.GetObject()))
	{
		Cast<IBehaviorState>(InnerState.GetObject())->JumpOrDrop();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::JumpOrDrop : InnerState Invalid"));
	}
}

void UBehaviorStateDecorator::ReleaseJump()
{
	if (IsValid(InnerState.GetObject()))
	{
		Cast<IBehaviorState>(InnerState.GetObject())->ReleaseJump();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::ReleaseJump : InnerState Invalid"));
	}
}

void UBehaviorStateDecorator::Dash()
{
	if (IsValid(InnerState.GetObject()))
	{
		Cast<IBehaviorState>(InnerState.GetObject())->Dash();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::Dash : InnerState Invalid"));
	}
}

void UBehaviorStateDecorator::WeakAttack()
{
	if (IsValid(InnerState.GetObject()))
	{
		Cast<IBehaviorState>(InnerState.GetObject())->WeakAttack();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::WeakAttack : InnerState Invalid"));
	}
}

void UBehaviorStateDecorator::StrongAttack()
{
	if (IsValid(InnerState.GetObject()))
	{
		Cast<IBehaviorState>(InnerState.GetObject())->StrongAttack();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::StrongAttack : InnerState Invalid"));
	}
}

void UBehaviorStateDecorator::Identity()
{
	if (IsValid(InnerState.GetObject()))
	{
		Cast<IBehaviorState>(InnerState.GetObject())->Identity();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::Identity : InnerState Invalid"));
	}
}

void UBehaviorStateDecorator::Defence()
{
	if (IsValid(InnerState.GetObject()))
	{
		Cast<IBehaviorState>(InnerState.GetObject())->Defence();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::Defence : InnerState Invalid"));
	}
}

void UBehaviorStateDecorator::PickingItem()
{
	if (IsValid(InnerState.GetObject()))
	{
		Cast<IBehaviorState>(InnerState.GetObject())->PickingItem();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBehaviorStateDecorator::PickingItem : InnerState Invalid"));
	}
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

