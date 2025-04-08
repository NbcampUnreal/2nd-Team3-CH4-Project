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

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UWrapStatusWidget> WrapStatusWidgetClass;

    void CreatePlayerWidgets();

    void LogMessage(const FString& Message);

    UPROPERTY(BlueprintReadWrite, Category = "UI")
    float BaseX = 50.f;

    UPROPERTY(BlueprintReadWrite, Category = "UI")
    float Spacing = 320.f;

    UPROPERTY(BlueprintReadWrite, Category = "UI")
    float DefaultY = -50.f;
};
