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
	
	// ...
}


// Called when the game starts
void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// Create Input State
	CharacterBehaviorState = NewObject<UCharacterBehaviorState>(this);
	if (!CharacterBehaviorState)
	{
		UE_LOG(LogTemp, Error, TEXT("UStateComponent::BeginPlay : BehaviorState Create Failed."));
		return;
	}

	// auto OuterCharacter = Cast<ACharacter>(GetOuter());
	// if (!OuterCharacter)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("UStateComponent::BeginPlay : OuterCharacter Cast Failed."));
	// 	return;
	// }
	//
	// UE_LOG(LogTemp, Display, TEXT("OuterCharacter DisplayName : %s"), *OuterCharacter->GetName());

	// Execute Binding Function
	OnStateComponentReady.Execute();

	// ...
}


// Called every frame
void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UCharacterBehaviorState* UStateComponent::GetBehaviorState()
{
	return CharacterBehaviorState;
}

