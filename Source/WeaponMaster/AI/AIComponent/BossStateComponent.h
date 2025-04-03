// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponMaster/AI/AIBossEnums.h"
#include "BossStateComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChangedSignature, EBossPhase, NewPhase);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEAPONMASTER_API UBossStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBossStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly, Category = "Phase")
	EBossPhase CurrentPhase;

	//UPROPERTY(BlueprintAssignable, Category = "Phase")
	//FOnPhaseChangedSignature OnPhaseChanged;

	// 현재 체력을 받아 페이즈 갱신
	void UpdatePhase(float CurrentHP, float MaxHP);
};
