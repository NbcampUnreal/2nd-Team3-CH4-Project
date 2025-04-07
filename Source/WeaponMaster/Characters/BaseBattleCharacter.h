// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSTCharacter.h"
#include "BaseBattleCharacter.generated.h"

struct FInputActionValue;

UCLASS()
class WEAPONMASTER_API ABaseBattleCharacter : public ASSTCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Additional Input Binding Functions
	void WeakAttack(const FInputActionValue& Value);
	void StrongAttack(const FInputActionValue& Value);
	void Identity(const FInputActionValue& Value);
	void Defence(const FInputActionValue& Value);
	void PickingItem(const FInputActionValue& Value);
	void MenuOnOff(const FInputActionValue& Value);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
};
