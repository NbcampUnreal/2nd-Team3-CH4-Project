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
	void OpenSingleWidget();
	void SetPlayerHP(float NewHP);
	void SetMenuWidget(bool bIsOpen);
protected:
	/**TODO :: 보기 안좋으면 Wraping */

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UPlayerStatusWidget> PlayerStatusWidgetClass;

	UPROPERTY()
	UPlayerStatusWidget* PlayerStatusWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UOptionMenuWidget> OptionMenuWidgetClass;

	UPROPERTY()
	UOptionMenuWidget* OptionMenuWidget;

	bool bIsOptionMenuVisible = false;

};
