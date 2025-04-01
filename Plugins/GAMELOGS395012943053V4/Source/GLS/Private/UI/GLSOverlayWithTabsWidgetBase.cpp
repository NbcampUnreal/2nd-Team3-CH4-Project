// Copyright (c) 2024 Fergius Engineering.

#include "GLSOverlayWithTabsWidgetBase.h"

#include "Common/GLSSettings.h"
#include "Utils/GLSUtils.h"

void UGLSOverlayWithTabsWidgetBase::InitNavigationRule()
{
    if (!SavedFocusRule.IsSet() && !UGLSUtils::IsEditorContext(this))
    {
        UUserInterfaceSettings* UISettings = GetMutableDefault<UUserInterfaceSettings>();
        SavedFocusRule = UISettings->RenderFocusRule;

        UISettings->RenderFocusRule = ERenderFocusRule::NavigationOnly;
    }
}

void UGLSOverlayWithTabsWidgetBase::InitInputMode()
{
    if (auto Player = GetOwningPlayer())
    {
        if (auto World = GetWorld())
        {
            if (World->GetNetMode() != ENetMode::NM_DedicatedServer && !EnabledInputMode.IsSet())
            {
                if (auto Settings = UGLSSettings::Get())
                {
                    switch (Settings->InputModeOverlay)
                    {
                    case EGLSInputModeOverlay::GameAndUI:
                        Player->SetInputMode(FInputModeGameAndUI());
                        break;
                    case EGLSInputModeOverlay::GameOnly:
                        Player->SetInputMode(FInputModeGameOnly());
                        break;
                    case EGLSInputModeOverlay::UIOnly:
                        Player->SetInputMode(FInputModeUIOnly());
                        break;
                    default:
                        break;
                    }

                    EnabledInputMode = Settings->InputModeOverlay;
                }
            }
        }
    }
}

void UGLSOverlayWithTabsWidgetBase::ResetNavigationRule()
{
    if (SavedFocusRule.IsSet() && !UGLSUtils::IsEditorContext(this))
    {
        UUserInterfaceSettings* UISettings = GetMutableDefault<UUserInterfaceSettings>();
        UISettings->RenderFocusRule = SavedFocusRule.GetValue();

        SavedFocusRule.Reset();
    }
}

void UGLSOverlayWithTabsWidgetBase::ResetInputMode()
{
    if (auto Player = GetOwningPlayer())
    {
        if (auto World = GetWorld())
        {
            if (World->GetNetMode() != ENetMode::NM_DedicatedServer && EnabledInputMode.IsSet())
            {
                EnabledInputMode.Reset();

                if (auto Settings = UGLSSettings::Get())
                {
                    switch (Settings->InputModeAfterOverlayHide)
                    {
                    case EGLSInputModeOverlay::GameAndUI:
                        Player->SetInputMode(FInputModeGameAndUI());
                        break;
                    case EGLSInputModeOverlay::GameOnly:
                        Player->SetInputMode(FInputModeGameOnly());
                        break;
                    case EGLSInputModeOverlay::UIOnly:
                        Player->SetInputMode(FInputModeUIOnly());
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
}

void UGLSOverlayWithTabsWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();

    InitNavigationRule();
    //NOTE: init input mode called from UGLSOverlaySubsystem::DisplayOverlay
}

void UGLSOverlayWithTabsWidgetBase::NativeDestruct()
{
    Super::NativeDestruct();

    ResetNavigationRule();
    ResetInputMode();
}