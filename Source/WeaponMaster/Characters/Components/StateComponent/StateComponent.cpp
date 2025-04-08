// Fill out your copyright notice in the Description page of Project Settings.

#include "StateComponent.h"
#include "States/CharacterInputState.h"
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

	InputState = NewObject<UCharacterInputState>(this);
	InputState->Initialize(Cast<ACharacter>(GetOuter()));

	// ...
	
}


// Called every frame
void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

