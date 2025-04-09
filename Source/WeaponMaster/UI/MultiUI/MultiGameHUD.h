// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiGameHUD.generated.h"

class UPlayerStatusWidget;
class UWrapStatusWidget;

UCLASS()
class WEAPONMASTER_API AMultiGameHUD : public AHUD
{
	GENERATED_BODY()
public:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    void TestDummyModule();

    UFUNCTION(BlueprintCallable)
    void TestChatModule(FString TestString, int32 TargetCharacterID);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UWrapStatusWidget> WrapStatusWidgetClass;

    TObjectPtr<UWrapStatusWidget> WrapStatusWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UWrapStatusWidget> ChatWidgetClass;

    TObjectPtr<UWrapStatusWidget> ChatWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UWrapStatusWidget> OptionMenuWidgetClass;

    TObjectPtr<UWrapStatusWidget> OptionMenuWidget;

    void CreatePlayerWidgets();

    void LogMessage(const FString& Message);

    UPROPERTY(EditAnywhere, Category = "UI")
    float BaseX = 50.f;

    UPROPERTY(EditAnywhere, Category = "UI")
    float Spacing = 320.f;

    UPROPERTY(EditAnywhere, Category = "UI")
    float DefaultY = -50.f;

    UPROPERTY(EditAnywhere, Category = "UI")
    int TestInt = 4;

};
