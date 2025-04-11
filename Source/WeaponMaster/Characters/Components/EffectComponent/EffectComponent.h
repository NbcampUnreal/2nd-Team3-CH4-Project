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
	// RPC호출로 서버 클라 둘다 변경해주기 때문에 Replicated하지 않아도 되겠다.(하면 Deactivate시 두번 삭제)
	UPROPERTY(/* Replicated, */VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TArray<EBehaviorEffect> ActiveBehaviorEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TMap<EBehaviorEffect, UBehaviorStateDecorator*> BehaviorEffectMapper;

	void Initialize();

	
	
	// Called when the game starts
	virtual void BeginPlay() override;

	// Replicate Setting
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InternalActivateBehaviorEffect(EBehaviorEffect BehaviorEffectType);
	void InternalActivateBehaviorEffectWithDuration(EBehaviorEffect BehaviorEffectType, float Duration);
	void InternalDeactivateBehaviorEffect(EBehaviorEffect BehaviorEffectType);
	
	UFUNCTION(Server, Reliable)
	void ServerActivateBehaviorEffect(EBehaviorEffect BehaviorEffectType);
	UFUNCTION(Server, Reliable)
	void ServerActivateBehaviorEffectWithDuration(EBehaviorEffect BehaviorEffectType, float Duration);
	UFUNCTION(Server, Reliable)
	void ServerDeactivateBehaviorEffect(EBehaviorEffect BehaviorEffectType);

	UFUNCTION(Client, Reliable)
	void ClientActivateBehaviorEffect(EBehaviorEffect BehaviorEffectType);
	UFUNCTION(Client, Reliable)
	void ClientActivateBehaviorEffectWithDuration(EBehaviorEffect BehaviorEffectType, float Duration);
	UFUNCTION(Client, Reliable)
	void ClientDeactivateBehaviorEffect(EBehaviorEffect BehaviorEffectType);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Effects")
	FORCEINLINE TArray<EBehaviorEffect>& GetActiveBehaviorEffects() { return ActiveBehaviorEffects; };
	
	void ActivateBehaviorEffect(EBehaviorEffect BehaviorEffectType);
	void ActivateBehaviorEffectWithDuration(EBehaviorEffect BehaviorEffectType, float Duration);
	void DeactivateBehaviorEffect(EBehaviorEffect BehaviorEffectType);

	
};
