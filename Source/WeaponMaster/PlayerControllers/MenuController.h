// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class WEAPONMASTER_API AMenuController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMenuController();

	// UI IMC, IA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* UIMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DirectionAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ChoiceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BackAction;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// Input Binding Functions
	void Direction(const FInputActionValue& Value);
	void Choice(const FInputActionValue& Value);
	void Back(const FInputActionValue& Value);
};
