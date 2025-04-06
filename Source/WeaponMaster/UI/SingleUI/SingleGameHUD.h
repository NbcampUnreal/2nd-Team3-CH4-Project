// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SingleGameHUD.generated.h"


class UPlayerStatusWidget;
class UOptionMenuWidget;

UCLASS()
class WEAPONMASTER_API ASingleGameHUD : public AHUD
{
	GENERATED_BODY()
public:
	ASingleGameHUD();

	virtual void BeginPlay() override;
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UPlayerStatusWidget> PlayerStatusWidgetClass;

	UPROPERTY()
	UPlayerStatusWidget* PlayerStatusWidget;

	/*UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UOptionMenuWidget> OptionMenuWidgetClass;

	UPROPERTY()
	UOptionMenuWidget* OptionMenuWidget;*/

	bool bIsOptionMenuVisible = false;

};
