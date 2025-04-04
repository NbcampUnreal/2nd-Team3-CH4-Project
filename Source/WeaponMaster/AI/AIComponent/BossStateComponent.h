// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponMaster/AI/AIBossEnums.h"
#include "BossStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChangedSignature, EBossPhase, NewPhase);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEAPONMASTER_API UBossStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBossStateComponent();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintReadOnly, Category = "Phase")
	EBossPhase CurrentPhase;

	UPROPERTY(BlueprintAssignable, Category = "Phase")
	FOnPhaseChangedSignature OnPhaseChanged;

	void UpdatePhase(float CurrentHP, float MaxHP);
};
