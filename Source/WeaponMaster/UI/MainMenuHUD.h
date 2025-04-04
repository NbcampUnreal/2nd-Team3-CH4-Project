// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

class UMainMenuWidget;
class UChoiceWidget;
class UUserWidget;

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

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowChoiceMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideChoiceMenu();

	UFUNCTION()
	void HandleNextClicked();

	UFUNCTION()
	void HandlePrevClicked();

	UFUNCTION()
	void ShowTestMenu();

protected:
	void LogMessage(const FString& Message);
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UMainMenuWidget> MainMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UChoiceWidget> ChoiceWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UChoiceWidget> ChoiceWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> TestWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UUserWidget> TestWidget;

};
