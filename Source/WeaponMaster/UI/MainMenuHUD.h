// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

class UMainMenuWidget;

UCLASS()
class WEAPONMASTER_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideMainMenu();

	UFUNCTION()
	void HandleSingleClicked();

	UFUNCTION()
	void HandleCoopClicked();

	UFUNCTION()
	void HandleVsClicked();

	UFUNCTION()
	void HandleShopClicked();

	UFUNCTION()
	void HandleExitClicked();


protected:
	void LogMessage(const FString& Message);
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UMainMenuWidget> MainMenuWidget;
};
