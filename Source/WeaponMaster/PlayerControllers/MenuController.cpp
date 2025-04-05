// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AMenuController::AMenuController()
{
	UIMappingContext = nullptr;	
	DirectionAction = nullptr;
	ChoiceAction = nullptr;
	BackAction = nullptr;
}

void AMenuController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (UIMappingContext)
		{
			Subsystem->AddMappingContext(UIMappingContext, 0);
		}
	}
}

void AMenuController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (DirectionAction)
		{
			EnhancedInputComponent->BindAction(DirectionAction, ETriggerEvent::Started, this, &AMenuController::Direction);
		}

		if (ChoiceAction)
		{
			EnhancedInputComponent->BindAction(ChoiceAction, ETriggerEvent::Started, this, &AMenuController::Choice);
		}

		if (BackAction)
		{
			EnhancedInputComponent->BindAction(BackAction, ETriggerEvent::Started, this, &AMenuController::Back);
		}
	}
}

void AMenuController::Direction(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AMenuController::Direction Binding Function"));
}

void AMenuController::Choice(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AMenuController::Choice Binding Function"));
}

void AMenuController::Back(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("AMenuController::Back Binding Function"));
}
