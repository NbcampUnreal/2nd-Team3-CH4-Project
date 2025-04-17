// Fill out your copyright notice in the Description page of Project Settings.

#include "StateComponent.h"
#include "States/CharacterBehaviorState.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UStateComponent::UStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bIsComponentReady = false;
	// ...
}

void UStateComponent::Move(const FInputActionValue& Value)
{
	if (auto InputState = Cast<IBehaviorState>(CharacterBehaviorState.GetObject()))
	{
		InputState->Move(Value);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::Move : Invalid InputState"));
	}
}

void UStateComponent::CrouchDrop()
{
	if (auto InputState = Cast<IBehaviorState>(CharacterBehaviorState.GetObject()))
	{
		InputState->CrouchDrop();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::CrouchDrop : Invalid InputState"));
	}
}

void UStateComponent::StopCrouchDrop()
{
	if (auto InputState = Cast<IBehaviorState>(CharacterBehaviorState.GetObject()))
	{
		InputState->StopCrouchDrop();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::StopCrouchDrop : Invalid InputState"));
	}
}

void UStateComponent::JumpOrDrop()
{
	if (auto InputState = Cast<IBehaviorState>(CharacterBehaviorState.GetObject()))
	{
		InputState->JumpOrDrop();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::JumpOrDrop : Invalid InputState"));
	}
}

void UStateComponent::ReleaseJump()
{
	if (auto InputState = Cast<IBehaviorState>(CharacterBehaviorState.GetObject()))
	{
		InputState->ReleaseJump();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::ReleaseJump : Invalid InputState"));
	}
}

void UStateComponent::Dash()
{
	if (auto InputState = Cast<IBehaviorState>(CharacterBehaviorState.GetObject()))
	{
		InputState->Dash();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::Dash : Invalid InputState"));
	}
}

void UStateComponent::WeakAttack()
{
	if (auto InputState = Cast<IBehaviorState>(CharacterBehaviorState.GetObject()))
	{
		InputState->WeakAttack();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::WeakAttack : Invalid InputState"));
	}
}

void UStateComponent::StrongAttack()
{
	if (auto InputState = Cast<IBehaviorState>(CharacterBehaviorState.GetObject()))
	{
		InputState->StrongAttack();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::StrongAttack : Invalid InputState"));
	}
}

void UStateComponent::Identity()
{
	if (auto InputState = Cast<IBehaviorState>(CharacterBehaviorState.GetObject()))
	{
		InputState->Identity();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::Identity : Invalid InputState"));
	}
}

void UStateComponent::Defence()
{
	if (auto InputState = Cast<IBehaviorState>(CharacterBehaviorState.GetObject()))
	{
		InputState->Defence();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::Defence : Invalid InputState"));
	}
}

void UStateComponent::PickingItem()
{
	if (auto InputState = Cast<IBehaviorState>(CharacterBehaviorState.GetObject()))
	{
		InputState->PickingItem();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::PickingItem : Invalid InputState"));
	}
}

// Called when the game starts
void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		return;
	}

	// Create Input State
	UCharacterBehaviorState* DefaultState = NewObject<UCharacterBehaviorState>(this);
    
	CharacterBehaviorState = TScriptInterface<UBehaviorState>(DefaultState);
    
	if (!IsValid(CharacterBehaviorState.GetObject()))
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::BeginPlay : BehaviorState Create Failed."));
		return;
	}
    
	if (OnStateComponentReady.IsBound())
	{
		OnStateComponentReady.Execute();
	}
	bIsComponentReady = true;
    
	// ...
}


// Called every frame
void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

