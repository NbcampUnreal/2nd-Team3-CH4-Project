// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectComponent.h"
#include "BehaviorEffects/BehaviorStateDecorator.h"
#include "BehaviorEffects/StunEffect.h"


// Sets default values for this component's properties
UEffectComponent::UEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


void UEffectComponent::Initialize()
{
	ActiveBehaviorEffects.Empty();
	BehaviorEffectMapper.Empty();

	BehaviorEffectMapper.Add(EBehaviorEffect::Stun, NewObject<UStunEffect>(this));
}

// Called when the game starts
void UEffectComponent::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}


// Called every frame
void UEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEffectComponent::ActivateBehaviorEffect(EBehaviorEffect BehaviorEffectType)
{
	if (auto EffectDecorator = BehaviorEffectMapper[BehaviorEffectType])
	{
		EffectDecorator->Activate();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEffectComponent::ActivateBehaviorEffect : Invalid Behavior Enum!"))
	}
}

void UEffectComponent::ActivateBehaviorEffect(EBehaviorEffect BehaviorEffectType, float Duration)
{
	if (auto EffectDecorator = BehaviorEffectMapper[BehaviorEffectType])
	{
		EffectDecorator->Activate(Duration);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEffectComponent::ActivateBehaviorEffect : Invalid Behavior Enum!"))
	}
}

void UEffectComponent::DeactivateBehaviorEffect(EBehaviorEffect BehaviorEffectType)
{
	if (auto EffectDecorator = BehaviorEffectMapper[BehaviorEffectType])
	{
		EffectDecorator->Deactivate();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEffectComponent::DeactivateBehaviorEffect : Invalid Behavior Enum!"))
	}
}

