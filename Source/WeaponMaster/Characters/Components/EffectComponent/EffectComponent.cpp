// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectComponent.h"
#include "BehaviorEffects/BehaviorStateDecorator.h"
#include "BehaviorEffects/SpecificEffects/ConfusedEffect.h"
#include "BehaviorEffects/SpecificEffects/DeathEffect.h"
#include "BehaviorEffects/SpecificEffects/SilenceEffect.h"
#include "BehaviorEffects/SpecificEffects/StiffnessEffect.h"
#include "BehaviorEffects/SpecificEffects/StunEffect.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UEffectComponent::UEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	
	// ...
}

void UEffectComponent::Initialize()
{
	ActiveBehaviorEffects.Empty();
	BehaviorEffectMapper.Empty();

	// Create BehaviorEffect Instances
	BehaviorEffectMapper.Add(EBehaviorEffect::Stun, NewObject<UStunEffect>(this));
	BehaviorEffectMapper.Add(EBehaviorEffect::Stiffness, NewObject<UStiffnessEffect>(this));   
	BehaviorEffectMapper.Add(EBehaviorEffect::Silence, NewObject<USilenceEffect>(this));     
	BehaviorEffectMapper.Add(EBehaviorEffect::Confused, NewObject<UConfusedEffect>(this));    
 
	// BehaviorEffectMapper.Add(EBehaviorEffect::UsingSkill, NewObject<UStunEffect>(this));  
	BehaviorEffectMapper.Add(EBehaviorEffect::Death, NewObject<UDeathEffect>(this));       
}

// Called when the game starts
void UEffectComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		return;
	}

	Initialize();
}

void UEffectComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UEffectComponent, ActiveBehaviorEffects, COND_SkipOwner);
}

// Called every frame
void UEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEffectComponent::InternalActivateBehaviorEffect(EBehaviorEffect BehaviorEffectType)
{
	if (auto EffectDecorator = BehaviorEffectMapper[BehaviorEffectType])
	{
		EffectDecorator->Activate();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEffectComponent::InternalActivateBehaviorEffect : Invalid Behavior Enum!"))
 	}
}

void UEffectComponent::InternalActivateBehaviorEffectWithDuration(EBehaviorEffect BehaviorEffectType, float Duration)
{
	if (auto EffectDecorator = BehaviorEffectMapper[BehaviorEffectType])
	{
		EffectDecorator->Activate(Duration);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEffectComponent::InternalActivateBehaviorEffectWithDuration : Invalid Behavior Enum!"))
	}
}

void UEffectComponent::InternalDeactivateBehaviorEffect(EBehaviorEffect BehaviorEffectType)
{
	if (auto EffectDecorator = BehaviorEffectMapper[BehaviorEffectType])
	{
		EffectDecorator->Deactivate();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEffectComponent::InternalDeactivateBehaviorEffect : Invalid Behavior Enum!"))
	}
}

void UEffectComponent::ActivateBehaviorEffect(EBehaviorEffect BehaviorEffectType)
{
	if (ENetMode NetMode = GetOwner()->GetNetMode(); NetMode == NM_Standalone)
	{
		// Standalone → 그냥 한번만 실행
		InternalActivateBehaviorEffect(BehaviorEffectType);
		return;
	}
	
	if (GetOwner()->HasAuthority())
	{
		InternalActivateBehaviorEffect(BehaviorEffectType);
		ClientActivateBehaviorEffect(BehaviorEffectType);
	}
	else
	{
		InternalActivateBehaviorEffect(BehaviorEffectType);
		ServerActivateBehaviorEffect(BehaviorEffectType);
	}
}

void UEffectComponent::ActivateBehaviorEffectWithDuration(EBehaviorEffect BehaviorEffectType, float Duration)
{
	if (ENetMode NetMode = GetOwner()->GetNetMode(); NetMode == NM_Standalone)
	{
		// Standalone → 그냥 한번만 실행
		InternalActivateBehaviorEffectWithDuration(BehaviorEffectType, Duration);
		return;
	}
	
	if (GetOwner()->HasAuthority())
	{
		InternalActivateBehaviorEffectWithDuration(BehaviorEffectType, Duration);
		ClientActivateBehaviorEffectWithDuration(BehaviorEffectType, Duration);
	}
	else
	{
		InternalActivateBehaviorEffectWithDuration(BehaviorEffectType, Duration);
		ServerActivateBehaviorEffectWithDuration(BehaviorEffectType, Duration);
	}
}

void UEffectComponent::DeactivateBehaviorEffect(EBehaviorEffect BehaviorEffectType)
{
	if (ENetMode NetMode = GetOwner()->GetNetMode(); NetMode == NM_Standalone)
	{
		// Standalone → 그냥 한번만 실행
		InternalActivateBehaviorEffect(BehaviorEffectType);
		return;
	}
	
	if (GetOwner()->HasAuthority())
	{
		InternalDeactivateBehaviorEffect(BehaviorEffectType);
		ClientDeactivateBehaviorEffect(BehaviorEffectType);
	}
	else
	{
		InternalDeactivateBehaviorEffect(BehaviorEffectType);
		ServerDeactivateBehaviorEffect(BehaviorEffectType);
	}
}

void UEffectComponent::ServerActivateBehaviorEffect_Implementation(EBehaviorEffect BehaviorEffectType)
{
	InternalActivateBehaviorEffect(BehaviorEffectType);
}

void UEffectComponent::ServerActivateBehaviorEffectWithDuration_Implementation(EBehaviorEffect BehaviorEffectType, float Duration)
{
	InternalActivateBehaviorEffectWithDuration(BehaviorEffectType, Duration);
}

void UEffectComponent::ServerDeactivateBehaviorEffect_Implementation(EBehaviorEffect BehaviorEffectType)
{
	InternalDeactivateBehaviorEffect(BehaviorEffectType);
}

void UEffectComponent::ClientActivateBehaviorEffect_Implementation(EBehaviorEffect BehaviorEffectType)
{
	InternalActivateBehaviorEffect(BehaviorEffectType);
}

void UEffectComponent::ClientActivateBehaviorEffectWithDuration_Implementation(EBehaviorEffect BehaviorEffectType, float Duration)
{
	InternalActivateBehaviorEffectWithDuration(BehaviorEffectType, Duration);
}

void UEffectComponent::ClientDeactivateBehaviorEffect_Implementation(EBehaviorEffect BehaviorEffectType)
{
	InternalDeactivateBehaviorEffect(BehaviorEffectType);
}

