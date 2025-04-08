// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IBaseBattleInputBindFunctions.h"
#include "SSTCharacter.h"
#include "BaseBattleCharacter.generated.h"

UCLASS()
class WEAPONMASTER_API ABaseBattleCharacter : public ASSTCharacter, public IBaseBattleInputBindFunctions
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStateComponent> StateComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Additional Input Binding Functions
	virtual void WeakAttack() override;
	virtual void StrongAttack() override;
	virtual void Identity() override;
	virtual void Defence() override;
	virtual void PickingItem() override;
	virtual void MenuOnOff() override;

	void BindInputFunctions();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
