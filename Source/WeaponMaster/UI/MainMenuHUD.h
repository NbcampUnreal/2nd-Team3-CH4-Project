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
	virtual void BeginPlay() override;

	// 메인 메뉴 관련 함수
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideMainMenu();

protected:
	// 이벤트 핸들러 (델리게이트 콜백)
	UFUNCTION()
	void HandleSingleClicked();

	UFUNCTION()
	void HandleMultiClicked();

	UFUNCTION()
	void HandleShopClicked();

	UFUNCTION()
	void HandleExitClicked();

	// 유틸리티 함수
	void LogMessage(const FString& Message);

	// 위젯 클래스 및 인스턴스
	UPROPERTY(EditDefaultsOnly, Category = "UI|Widgets")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "UI|Widgets")
	TObjectPtr<UMainMenuWidget> MainMenuWidget;
};