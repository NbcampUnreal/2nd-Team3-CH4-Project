// Copyright (c) 2024 Fergius Engineering.

#include "GLSOverlayWidgetBase.h"

#include <Components/Widget.h>
#include <Misc/CoreDelegates.h>

#include "GLSSaveSubsystem.h"
#include "GLSSaveSystemTypes.h"
#include "GLSSaveSystemUtils.h"
#include "GLSWidgetBase.h"

DEFINE_LOG_CATEGORY_STATIC(GLSOverlayWidget, Log, All);

UGLSOverlayWidgetBase::UGLSOverlayWidgetBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsFocusable(true);
}

void UGLSOverlayWidgetBase::OnEnterBackground()
{
    SaveLayoutData();
}

void UGLSOverlayWidgetBase::SaveLayoutData()
{
    for (auto Object : InterfaceArray)
    {
        IGLSSaveSystemInterface* Interface = Cast<IGLSSaveSystemInterface>(Object);
        UUserWidget* Widget = Cast<UUserWidget>(Object);
        if (Interface)
        {
            const FString TabName = Interface->GetParentTabName();
            if (!TabName.IsEmpty())
            {
                const TMap<FString, FString> Params = Interface->Execute_PrepareDataToSave(Widget);
                if (!Params.IsEmpty())
                {
                    UGLSSaveSystemUtils::SaveParamsByTabName(Widget, TabName, Params);
                }
            }
        }
    }

    if (auto Subsystem = UGLSSaveSubsystem::Get(this))
    {
        Subsystem->SaveDataBinary();
    }
}

void UGLSOverlayWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();

    UGLSSaveSystemUtils::GetSaveSystemInrerfacesRecursive(this, InterfaceArray);

    FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &UGLSOverlayWidgetBase::OnEnterBackground);
}

void UGLSOverlayWidgetBase::NativeDestruct()
{
    SaveLayoutData();

    Super::NativeDestruct();
}