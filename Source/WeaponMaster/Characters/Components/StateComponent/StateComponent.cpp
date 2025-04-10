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
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->Move(Value);
}

void UStateComponent::CrouchDrop()
{
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->CrouchDrop();
}

void UStateComponent::StopCrouchDrop()
{
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->StopCrouchDrop();
}

void UStateComponent::JumpOrDrop()
{
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->JumpOrDrop();
}

void UStateComponent::ReleaseJump()
{
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->ReleaseJump();
}

void UStateComponent::Dash()
{
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->Dash();
}

void UStateComponent::WeakAttack()
{
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->WeakAttack();
}

void UStateComponent::StrongAttack()
{
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->StrongAttack();
}

void UStateComponent::Identity()
{
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->Identity();
}

void UStateComponent::Defence()
{
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->Defence();
}

void UStateComponent::PickingItem()
{
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->PickingItem();
}

void UStateComponent::MenuOnOff()
{
	Cast<IBehaviorState>(CharacterBehaviorState.GetObject())->MenuOnOff();
}


// Called when the game starts
void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// Create Input State
	UCharacterBehaviorState* DefaultState = NewObject<UCharacterBehaviorState>(this);
    
	CharacterBehaviorState = TScriptInterface<UBehaviorState>(DefaultState);
    
	if (!IsValid(CharacterBehaviorState.GetObject()))
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::BeginPlay : BehaviorState Create Failed."));
		return;
	}

	auto OuterCharacter = Cast<ACharacter>(GetOuter());
	if (!OuterCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::BeginPlay : OuterCharacter Cast Failed."));
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

