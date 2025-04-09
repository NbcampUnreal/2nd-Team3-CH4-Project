// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/StatusTypes.h"
#include "Components/ActorComponent.h"
#include "EffectComponent.generated.h"

class UBehaviorStateDecorator;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WEAPONMASTER_API UEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEffectComponent();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TArray<EBehaviorEffect> ActiveBehaviorEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TMap<EBehaviorEffect, UBehaviorStateDecorator*> BehaviorEffectMapper;

	void Initialize();
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Effects")
	FORCEINLINE TArray<EBehaviorEffect>& GetActiveBehaviorEffects() { return ActiveBehaviorEffects; };
	
	void ActivateBehaviorEffect(EBehaviorEffect BehaviorEffectType);
	void ActivateBehaviorEffect(EBehaviorEffect BehaviorEffectType, float Duration);
	void DeactivateBehaviorEffect(EBehaviorEffect BehaviorEffectType);
};
