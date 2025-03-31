// Copyright (c) 2024 Fergius Engineering.

#include "GLSSettings.h"

#include <Blueprint/UserWidget.h>
#include <Modules/ModuleManager.h>

#include "GLSTypes.h"

UGLSSettings::UGLSSettings()
    : Super()
{
    GLSMainClass = LoadClass<UUserWidget>(nullptr, TEXT("/GameLogsSystem/WBP_GLS_Overlay_WithTabs.WBP_GLS_Overlay_WithTabs_C"));
    check(GLSMainClass);

    DefaultLoggingTagTypes |= (uint8)EGLSTagsDefaultTypes::Pie;
    DefaultLoggingTagTypes |= (uint8)EGLSTagsDefaultTypes::Role;
    DefaultLoggingTagTypes |= (uint8)EGLSTagsDefaultTypes::Function;
    DefaultLoggingTagTypes |= (uint8)EGLSTagsDefaultTypes::PlayerName;

    CustomCategoriesToLoad |= (uint8)EGLSCustomCategoriesTypes::Categories;

    OverlayInputMappings.Add(FInputActionKeyMapping(TEXT("DisplayGLSOverlay"), EKeys::G, true, false, true));
    OverlayInputMappings.Add(FInputActionKeyMapping(TEXT("DisplayGLSOverlay"), EKeys::Gamepad_Special_Right));
    OverlayInputMappings.Add(FInputActionKeyMapping(TEXT("DisplayGLSOverlay"), EKeys::TouchKeys[1]));
}

UGLSSettings* UGLSSettings::Get()
{
#if WITH_RELOAD
    if (IsReloadActive() && GetActiveReloadType() != EActiveReloadType::Reinstancing)
    {
        return nullptr;
    }
#endif

    if (IsEngineExitRequested())
    {
        return nullptr;
    }

    auto Settings = GetMutableDefault<UGLSSettings>();
    if (IsValid(Settings) && Settings->IsValidLowLevelFast())
    {
        return Settings;
    }

    return nullptr;
}