// Copyright (c) 2024 Fergius Engineering.

#include "GLSOverlaySubsystem.h"

#include <Components/InputComponent.h>
#include <Engine/GameInstance.h>
#include <Engine/World.h>
#include <Framework/Application/SlateApplication.h>
#include <GameFramework/GameStateBase.h>
#include <Kismet/GameplayStatics.h>
#include <Templates/SharedPointer.h>
#include <Widgets/SWidget.h>

#include "GLSInputProcessor.h"
#include "GLSLogModelsPoolSubsystem.h"
#include "GLSOverlayWithTabsWidgetBase.h"
#include "GLSSettings.h"
#include "GLSSubsystem.h"
#include "GLSUtils.h"

void UGLSOverlaySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.ActivateExternalSubsystem(UGLSSubsystem::StaticClass());

    TimerManager = MakeUnique<FTimerManager>();

    FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UGLSOverlaySubsystem::OnPostWorldInitialization);
    FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UGLSOverlaySubsystem::OnWorldBeginTearDown);

    InitializeInput();
}

bool UGLSOverlaySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return FGLSUtils::IsGLSEnabled();
}

UGLSOverlaySubsystem* UGLSOverlaySubsystem::Get(const UObject* Context)
{
    if (IsValid(Context) && Context->IsValidLowLevelFast())
    {
        if (auto World = Context->GetWorld())
        {
            if (auto GameInstance = World->GetGameInstance())
            {
                return GameInstance->GetSubsystem<UGLSOverlaySubsystem>();
            }
        }
    }

    return nullptr;
}

void UGLSOverlaySubsystem::InitializeInput()
{
    if (auto Settings = UGLSSettings::Get())
    {
        TSharedPtr<FGLSInputProcessor> GLSInputProcessor = MakeShared<FGLSInputProcessor>();
        GLSInputProcessor->OverlayInputMappings = Settings->OverlayInputMappings;
        GLSInputProcessor->OnInputOverlayPressed.AddUObject(this, &UGLSOverlaySubsystem::OnDisplayOverlayPressed);
        GLSInputProcessor->OnInputOverlayReleased.AddUObject(this, &UGLSOverlaySubsystem::OnDisplayOverlayReleased);

        FSlateApplication::Get().RegisterInputPreProcessor(GLSInputProcessor);
    }
}

void UGLSOverlaySubsystem::OnDisplayOverlayPressed()
{
    if (TimerManager->IsTimerActive(DisplayOverlay_TimerHandle))
    {
        return;
    }

    if (CanDisplayOverlay())
    {
        if (auto Settings = UGLSSettings::Get())
        {
            const float Delay = GetPressDelayForDisplay();
            if (Delay <= 0.f)
            {
                DisplayOverlay();
            }
            else
            {
                TimerManager->SetTimer(DisplayOverlay_TimerHandle, FTimerDelegate::CreateUObject(this, &UGLSOverlaySubsystem::DisplayOverlay), Delay, false);
            }
        }
    }
}

void UGLSOverlaySubsystem::OnDisplayOverlayReleased()
{
    if (TimerManager->IsTimerActive(DisplayOverlay_TimerHandle))
    {
        TimerManager->ClearTimer(DisplayOverlay_TimerHandle);
    }
}

void UGLSOverlaySubsystem::OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
    if (bOverlayWasDisplayed)
    {
        bOverlayWasDisplayed = false;
        DisplayOverlayAfterWorldTearDown();
    }
}

void UGLSOverlaySubsystem::OnWorldBeginTearDown(UWorld* World)
{
    if (bOverlayDisplayed)
    {
        bOverlayWasDisplayed = true;
    }

    LastFocusedWidget = nullptr;
    HideOverlay();
    OverlayWidget = nullptr;
}

void UGLSOverlaySubsystem::DisplayOverlay()
{
    if (!IsValid(this))
    {
        return;
    }

    if (bOverlayDisplayed)
    {
        return;
    }

    SaveLastFocusedWidget();

    if (IsValid(OverlayWidget) && OverlayWidget->IsValidLowLevel())
    {
        OverlayWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        OverlayWidget->SetFocus();
        OverlayWidget->AddToViewport(9999);
        OverlayWidget->SetIsEnabled(true);
        OverlayWidget->InitNavigationRule();
        OverlayWidget->InitInputMode();
    }
    else
    {
        if (auto Settings = UGLSSettings::Get())
        {
            auto WidgetClass = Settings->GLSMainClass;
            if (WidgetClass != nullptr)
            {
                auto World = GetWorld();
                if (IsValid(World) && World->IsValidLowLevel() && !World->bIsTearingDown)
                {
                    if (auto Widget = CreateWidget<UGLSOverlayWithTabsWidgetBase>(World, WidgetClass))
                    {
                        Widget->AddToViewport(9999); //TODO: mode the param ZOrder to settings
                        Widget->SetFocus();

                        OverlayWidget = Widget;
                        OverlayWidget->InitInputMode();
                    }
                }
            }
        }
    }

    bOverlayDisplayed = true;
}

void UGLSOverlaySubsystem::HideOverlay()
{
    if (!IsValid(this))
    {
        return;
    }

    if (!bOverlayDisplayed)
    {
        return;
    }

    if (IsValid(OverlayWidget) && OverlayWidget->IsInViewport())
    {
        OverlayWidget->SetVisibility(ESlateVisibility::Collapsed);
        OverlayWidget->SetIsEnabled(false);
        OverlayWidget->ResetNavigationRule();
        OverlayWidget->ResetInputMode();
    }

    UGLSLogModelsPoolSubsystem::Get(this)->ResetModels();

    //NOTE : A delay is necessary because the focus is not always set to the element that was in focus before the panel was opened
    if (auto World = GetWorld())
    {
        if (LastFocusedWidget.IsValid())
        {
            FTimerHandle Handle;
            TimerManager->SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this]() { SetWidgetToLastFocused(); }), 0.25f, false);
        }
    }

    bOverlayDisplayed = false;
}

bool UGLSOverlaySubsystem::CanDisplayOverlay()
{
    if (bOverlayDisplayed)
    {
        return false;
    }

    if (auto Settings = UGLSSettings::Get())
    {
        if (Settings->OverlayInputMappings.IsEmpty() || !Settings->bAllowDisplayOverlayByInput)
        {
            return false;
        }

        auto World = GetWorld();
        if (!IsValid(World) || World->bIsTearingDown)
        {
            return false;
        }

        bool bIgnoreByGameModeClass = false;
        bool bIgnoreByLevel = false;
        for (const auto& Class : Settings->IgnoredGameModesForOverlay)
        {
            if (Class == World->GetGameState()->GameModeClass)
            {
                bIgnoreByGameModeClass = true;
                break;
            }
        }

        for (const auto& LevelSoftRef : Settings->IgnoredLevelsForOverlay)
        {
            if (LevelSoftRef.GetAssetName() == World->GetName())
            {
                bIgnoreByLevel = true;
                break;
            }
        }

        if (bIgnoreByGameModeClass || bIgnoreByLevel)
        {
            return false;
        }
    }

    return FGLSUtils::IsGLSEnabled();
}

void UGLSOverlaySubsystem::SaveLastFocusedWidget()
{
    LastFocusedWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();
}

void UGLSOverlaySubsystem::SetWidgetToLastFocused()
{
    if (LastFocusedWidget.IsValid())
    {
        FSlateApplication::Get().SetKeyboardFocus(LastFocusedWidget, EFocusCause::OtherWidgetLostFocus);

        LastFocusedWidget = nullptr;
    }
}

float UGLSOverlaySubsystem::GetPressDelayForDisplay()
{
    if (auto Settings = UGLSSettings::Get())
    {
        if (UGLSUtils::IsDesktopPlatform())
        {
            return Settings->OverlayDisplayDelaySeconds;
        }
        if (UGLSUtils::IsMobilePlatform())
        {
            return Settings->OverlayDisplayDelayMobileSeconds;
        }
        if (UGLSUtils::IsConsolePlatform())
        {
            return Settings->OverlayDisplayDelayConsoleSeconds;
        }
    }

    return 0;
}

void UGLSOverlaySubsystem::DisplayOverlayAfterWorldTearDown()
{
    if (const auto World = GetWorld())
    {
        if (World->GetNumPlayerControllers() == 0 || !World->GetGameState())
        {
            FTimerHandle Handle;
            TimerManager->SetTimer(Handle, this, &UGLSOverlaySubsystem::DisplayOverlayAfterWorldTearDown, 0.25f, false);
        }
        else
        {
            if (CanDisplayOverlay())
            {
                DisplayOverlay();
            }
        }
    }
}

void UGLSOverlaySubsystem::Tick(float DeltaTime)
{
    if (TimerManager)
    {
        TimerManager->Tick(DeltaTime);
    }
}

TStatId UGLSOverlaySubsystem::GetStatId() const
{
    return TStatId();
}