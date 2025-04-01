// Copyright (c) 2024 Fergius Engineering.

#pragma once

#include <Engine/TimerHandle.h>
#include <Engine/World.h>
#include <Subsystems/GameInstanceSubsystem.h>
#include <Tickable.h>
#include <TimerManager.h>

#include "GLSOverlaySubsystem.generated.h"

UCLASS()
class GLS_API UGLSOverlaySubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, DisplayName = "Get Game Logs Overlay", meta = (HidePin = "Context", DefaultToSelf = "Context"), Category = GLS)
    static UGLSOverlaySubsystem* Get(const UObject* Context);

    UFUNCTION(BlueprintCallable, Category = GLS)
    void DisplayOverlay();

    UFUNCTION(BlueprintCallable, Category = GLS)
    void HideOverlay();

private:
    TUniquePtr<FTimerManager> TimerManager;

    FTimerHandle DisplayOverlay_TimerHandle;

    UPROPERTY(transient)
    TObjectPtr<class UGLSOverlayWithTabsWidgetBase> OverlayWidget = nullptr;

    TSharedPtr<class SWidget> LastFocusedWidget = nullptr;

    bool bOverlayDisplayed = false;

    /** Uses to recreate the GLS overlay after new world creation */
    bool bOverlayWasDisplayed = false;

private:
    bool IsTickableWhenPaused() const override { return true; };

    TStatId GetStatId() const override;

    void Tick(float DeltaTime) override;

    bool ShouldCreateSubsystem(UObject* Outer) const override;

    void Initialize(FSubsystemCollectionBase& Collection) override;

    void InitializeInput();

    bool CanDisplayOverlay();

    void SaveLastFocusedWidget();

    void SetWidgetToLastFocused();

    float GetPressDelayForDisplay();

    void DisplayOverlayAfterWorldTearDown();

private:
    void OnDisplayOverlayPressed();

    void OnDisplayOverlayReleased();

    void OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);

    void OnWorldBeginTearDown(UWorld* World);
};