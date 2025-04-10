// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiGameHUD.generated.h"

class USessionWidget;
class UPlayerNameWidget;
class USessionLobbyWidget;
class UPlayerStatusWidget;
class UWrapStatusWidget;
class UChatWidget;
class UOptionMenuWidget;

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

    UFUNCTION(BlueprintCallable)
    void SetHPModule(float NewHP, int32 TargetCharacterID);
    
    UFUNCTION(BlueprintCallable)
    void SetMenuWidget(bool bIsOpen);

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UWrapStatusWidget> WrapStatusWidgetClass;

    TObjectPtr<UWrapStatusWidget> WrapStatusWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UChatWidget> ChatWidgetClass;

    TObjectPtr<UChatWidget> ChatWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UOptionMenuWidget> OptionMenuWidgetClass;

    TObjectPtr<UOptionMenuWidget> OptionMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SessionLobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<USessionLobbyWidget> SessionLobbyWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MapSelectWidgetClass;

	UPROPERTY()
	TObjectPtr<USessionWidget> MapSelectWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PlayerNameWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayerNameWidget> PlayerNameWidget;

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
