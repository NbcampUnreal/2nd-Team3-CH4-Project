// Copyright (c) 2024 Fergius Engineering.

#include "GLSSaveSystemUtils.h"

#include <Blueprint/WidgetTree.h>
#include <Components/Widget.h>
#include <Engine/EngineTypes.h>
#include <Engine/World.h>
#include <Serialization/BufferArchive.h>
#include <Serialization/MemoryReader.h>
#include <Serialization/StructuredArchive.h>

#include "GLSSaveSubsystem.h"
#include "GLSSaveSystemConstants.h"
#include "GLSSaveSystemTypes.h"
#include "GLSWidgetBase.h"

FString UGLSSaveSystemUtils::MakeSaveParamName(UObject* Context, const FString& VariableName, const FString& ParamName)
{
    if (IsValid(Context) && Context->IsValidLowLevelFast() && IsValid(Context->GetWorld()) && Context->GetWorld()->IsValidLowLevel())
    {
        const FString ClassName = Context->GetClass()->GetName();
        return FString::Format(TEXT("{0}~{1}~{2}"), {*ClassName, VariableName, ParamName});
    }

    return {};
}

void UGLSSaveSystemUtils::SaveParamsByTabName(UObject* Context, const FString& TabName, const TMap<FString, FString>& Params)
{
    if (auto Subsystem = UGLSSaveSubsystem::Get(Context))
    {
        if (IsValid(Context->GetWorld()) && ensure(!TabName.IsEmpty()) && Params.Num() > 0)
        {
            Subsystem->SaveParams(Context->GetWorld()->WorldType, FGLSUtils::GetPIEInstance(Context), TabName, Params);
        }
    }
}

bool UGLSSaveSystemUtils::GetSaveValueAsString(UObject* Context, const FString& ParamName, FString& OutString)
{
    OutString.Empty();

    if (IsValid(Context) && Context->IsValidLowLevelFast() && IsValid(Context->GetWorld()))
    {
        IGLSSaveSystemInterface* Interface = Cast<IGLSSaveSystemInterface>(Context);
        if (ensure(Interface))
        {
            const FString ParentTabName = Interface->GetParentTabName();
            if (auto Subsystem = UGLSSaveSubsystem::Get(Context))
            {
                return Subsystem->GetParamValue(Context->GetWorld()->WorldType, FGLSUtils::GetPIEInstance(Context), ParentTabName, ParamName, OutString);
            }
        }
    }

    return {};
}

bool UGLSSaveSystemUtils::GetSaveValueAsInteger(UObject* Context, const FString& ParamName, int32& OutValue)
{
    FString Value;
    if (UGLSSaveSystemUtils::GetSaveValueAsString(Context, ParamName, Value))
    {
        OutValue = FCString::Atoi(*Value);
        return true;
    }

    return false;
}

bool UGLSSaveSystemUtils::GetSaveValueAsBoolean(UObject* Context, const FString& ParamName, bool& OutValue)
{
    FString Value;
    if (UGLSSaveSystemUtils::GetSaveValueAsString(Context, ParamName, Value))
    {
        ensure(Value == TEXT("true") || Value == TEXT("false") || Value == TEXT("1") || Value == TEXT("0"));

        if (Value == TEXT("true"))
        {
            OutValue = true;
        }
        else if (Value == TEXT("false"))
        {
            OutValue = false;
        }
        else
        {
            OutValue = (bool)FCString::Atoi(*Value);
        }

        return true;
    }

    return false;
}

TMap<FString, FString> UGLSSaveSystemUtils::GetSaveValuesBySaveTag(UObject* Context, const TArray<FString>& SaveTags)
{
    if (auto Subsystem = UGLSSaveSubsystem::Get(Context))
    {
        if (IGLSSaveSystemInterface* Interface = Cast<IGLSSaveSystemInterface>(Context))
        {
            const FString ParentTabName = Interface->GetParentTabName();
            return Subsystem->GetSaveValuesBySaveTag(Context->GetWorld()->WorldType, FGLSUtils::GetPIEInstance(Context), ParentTabName, SaveTags);
        }
    }

    return {};
}

void UGLSSaveSystemUtils::SetParentTabNameRecursive(UWidget* Widget, const FString& TabName)
{
    if (!IsValid(Widget))
        return;

    IGLSSaveSystemInterface* Interface = Cast<IGLSSaveSystemInterface>(Widget);
    if (Interface)
    {
        Interface->SetParentTabName(TabName);
        Interface->Execute_RequestApplySave(Widget);
    }

    if (UUserWidget* UserWidget = Cast<UUserWidget>(Widget))
    {
        UWidgetTree* _WidgetTree = UserWidget->WidgetTree;
        TArray<UWidget*> AllWidgets;
        _WidgetTree->GetAllWidgets(AllWidgets);

        for (UWidget* Child : AllWidgets)
        {
            SetParentTabNameRecursive(Child, TabName);
        }
    }
}

void UGLSSaveSystemUtils::RequestSaveDataRecursive(class UWidget* Widget)
{
    if (!IsValid(Widget) || !Widget->IsValidLowLevel())
        return;

    IGLSSaveSystemInterface* Interface = Cast<IGLSSaveSystemInterface>(Widget);
    if (Interface)
    {
        const FString TabName = Interface->GetParentTabName();
        const TMap<FString, FString> Params = Interface->Execute_PrepareDataToSave(Widget);
        ensure(!TabName.IsEmpty());

        UGLSSaveSystemUtils::SaveParamsByTabName(Widget, TabName, Params);
    }

    if (UUserWidget* UserWidget = Cast<UUserWidget>(Widget))
    {
        UWidgetTree* _WidgetTree = UserWidget->WidgetTree;
        TArray<UWidget*> AllWidgets;
        _WidgetTree->GetAllWidgets(AllWidgets);

        for (UWidget* Child : AllWidgets)
        {
            RequestSaveDataRecursive(Child);
        }
    }
}

void UGLSSaveSystemUtils::GetSaveSystemInrerfacesRecursive(class UWidget* Widget, TArray<TWeakObjectPtr<UObject>>& OutInterfaceArray)
{
    if (!IsValid(Widget) || !Widget->IsValidLowLevel())
        return;

    IGLSSaveSystemInterface* Interface = Cast<IGLSSaveSystemInterface>(Widget);
    if (Interface)
    {
        OutInterfaceArray.Add(Widget);
    }

    if (UUserWidget* UserWidget = Cast<UUserWidget>(Widget))
    {
        UWidgetTree* _WidgetTree = UserWidget->WidgetTree;
        TArray<UWidget*> AllWidgets;
        _WidgetTree->GetAllWidgets(AllWidgets);

        for (UWidget* Child : AllWidgets)
        {
            GetSaveSystemInrerfacesRecursive(Child, OutInterfaceArray);
        }
    }
}

void UGLSSaveSystemUtils::RemoveSaveDataByTabName(UObject* Context, const FString& TabName)
{
    if (auto Subsystem = UGLSSaveSubsystem::Get(Context))
    {
        if (IsValid(Context->GetWorld()))
        {
            Subsystem->RemoveSave(Context->GetWorld()->WorldType, FGLSUtils::GetPIEInstance(Context), TabName);
        }
    }
}

void UGLSSaveSystemUtils::CleanSaveDataByTabName(UObject* Context, const FString& TabName)
{
    if (auto Subsystem = UGLSSaveSubsystem::Get(Context))
    {
        if (IsValid(Context->GetWorld()))
        {
            Subsystem->CleanSave(Context->GetWorld()->WorldType, FGLSUtils::GetPIEInstance(Context), TabName);
        }
    }
}

void UGLSSaveSystemUtils::EnableSaveDataByTabName(UObject* Context, const FString& TabName, const bool bEnable, const bool bForceCleanParams)
{
    if (auto Subsystem = UGLSSaveSubsystem::Get(Context))
    {
        if (IsValid(Context->GetWorld()))
        {
            Subsystem->EnableSaveData(Context->GetWorld()->WorldType, FGLSUtils::GetPIEInstance(Context), TabName, bEnable, bForceCleanParams);
        }
    }
}

bool UGLSSaveSystemUtils::GetEnableSaveDataByTabName(const UObject* Context, const FString& TabName)
{
    if (auto Subsystem = UGLSSaveSubsystem::Get(Context))
    {
        if (IsValid(Context->GetWorld()))
        {
            return Subsystem->GetEnableSaveData(Context->GetWorld()->WorldType, FGLSUtils::GetPIEInstance(const_cast<UObject*>(Context)), TabName);
        }
    }

    return false;
}

TArray<FString> UGLSSaveSystemUtils::GetTabNamesFromSave(UObject* Context)
{
    if (auto Subsystem = UGLSSaveSubsystem::Get(Context))
    {
        if (IsValid(Context->GetWorld()))
        {
            return Subsystem->GetTabNames(Context->GetWorld()->WorldType, FGLSUtils::GetPIEInstance(Context));
        }
    }

    return {};
}

FString UGLSSaveSystemUtils::GetSelectedTabName(UObject* Context)
{
    if (auto Subsystem = UGLSSaveSubsystem::Get(Context))
    {
        return Subsystem->GetSelectedTabName(Context->GetWorld()->WorldType, FGLSUtils::GetPIEInstance(Context));
    }

    return {};
}

void UGLSSaveSystemUtils::SetSelectedTabByName(UObject* Context, const FString& TabName, const bool bSelected)
{
    if (auto Subsystem = UGLSSaveSubsystem::Get(Context))
    {
        Subsystem->SetSelectedTabByName(Context->GetWorld()->WorldType, FGLSUtils::GetPIEInstance(Context), TabName, bSelected);
    }
}
