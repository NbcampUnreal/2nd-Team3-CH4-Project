// Copyright (c) 2024 Fergius Engineering.

#include "GLSSaveSubsystem.h"

#include <Blueprint/UserWidget.h>
#include <Blueprint/WidgetBlueprintLibrary.h>
#include <JsonObjectConverter.h>
#include <Kismet/GameplayStatics.h>
#include <Misc/FileHelper.h>
#include <Misc/Paths.h>

#include "GLSMacroses.h"
#include "GLSOverlayWidgetBase.h"
#include "GLSSaveSystemConstants.h"
#include "GLSSaveSystemUtils.h"
#include "GLSWidgetBase.h"

DEFINE_LOG_CATEGORY_STATIC(GLSSave, Log, All);

UGLSSaveSubsystem* UGLSSaveSubsystem::Get(const UObject* Context)
{
    if (IsInGameThread() && IsValid(Context) && Context->IsValidLowLevelFast())
    {
        if (UGLSSaveSubsystem* SaveSubsystem = GEngine->GetEngineSubsystem<UGLSSaveSubsystem>())
        {
            SaveSubsystem->LoadSaveDataBinary();
            return SaveSubsystem;
        }
    }

    return nullptr;
}

void UGLSSaveSubsystem::SaveParams(const EWorldType::Type Type, const int32 Instance, const FString& TabName, const TMap<FString, FString>& Params)
{
    if (!TabName.IsEmpty())
    {
        FGLSSaveTabParamsInfo& TabParamsInfo = GetTabParamsInfoRef(Type, Instance, TabName);

        TArray<FString> KeysToRemove;
        for (const auto& Pair : Params)
        {
            if (auto Settings = UGLSSettings::Get())
            {
                // NOTE: if value == 0, its false and need remove from settings
                // all "false", "true", "1" and other need allow to save
                if (Pair.Value == TEXT("0") || !Settings->bEnableLayoutSave)
                {
                    KeysToRemove.Add(Pair.Key);
                }
                else
                {
                    //NOTE : allow get and save params only if auto save enabled
                    if (!TabParamsInfo.bEnableAutoSaveParams)
                    {
                        continue;
                    }

                    TabParamsInfo.Params.Add(Pair);
                }
            }
        }

        for (const FString& Key : KeysToRemove)
        {
            TabParamsInfo.Params.Remove(Key);
        }
    }

    //SaveDataBinary();
}

bool UGLSSaveSubsystem::GetParamValue(const EWorldType::Type Type, const int32 Instance, const FString& TabName, const FString& ParamName, FString& OutValue)
{
    OutValue = {};

    FGLSSaveTabParamsInfo& TabParamsInfo = GetTabParamsInfoRef(Type, Instance, TabName);

    if (auto Settings = UGLSSettings::Get())
    {
        //NOTE : allow get and save params only if auto save enabled
        if (!TabParamsInfo.bEnableAutoSaveParams || !Settings->bEnableLayoutSave)
        {
            return false;
        }

        if (FString* Param = TabParamsInfo.Params.Find(ParamName))
        {
            OutValue = *Param;
            return true;
        }
    }

    return false;
}

TMap<FString, FString> UGLSSaveSubsystem::GetSaveValuesBySaveTag(const EWorldType::Type Type, const int32 Instance, const FString& TabName, const TArray<FString>& SaveTags)
{
    TMap<FString, FString> ReturnMap;

    const auto& TabInfo = GetTabParamsInfoRef(Type, Instance, TabName);
    for (const auto& Pair : TabInfo.Params)
    {
        bool bMatches = true;
        for (const auto& SaveTag : SaveTags)
        {
            if (!Pair.Key.Contains(SaveTag))
            {
                bMatches = false;
                break;
            }
        }

        if (bMatches)
        {
            ReturnMap.Add(Pair);
        }
    }

    return ReturnMap;
}

void UGLSSaveSubsystem::RemoveSave(const EWorldType::Type Type, const int32 Instance, const FString& TabName)
{
    FGLSSaveTabContext& TabContext = GetTabContextRef(Type, Instance);
    TabContext.TabParamsInfo.RemoveAll([&](const FGLSSaveTabParamsInfo& Info)
        {
            return TabName == Info.TabName;
        });

    //SaveDataBinary();
}

void UGLSSaveSubsystem::CleanSave(const EWorldType::Type Type, const int32 Instance, const FString& TabName)
{
    FGLSSaveTabContext& TabContext = GetTabContextRef(Type, Instance);
    auto Tab = TabContext.TabParamsInfo.FindByPredicate([&](const FGLSSaveTabParamsInfo& Info)
        {
            return TabName == Info.TabName;
        });

    if (Tab)
    {
        Tab->Params.Empty();
    }

    //SaveDataBinary();
}

void UGLSSaveSubsystem::EnableSaveData(const EWorldType::Type Type, const int32 Instance, const FString& TabName, const bool bEnable, const bool bForceCleanParams)
{
    FGLSSaveTabContext& TabContext = GetTabContextRef(Type, Instance);
    auto Tab = TabContext.TabParamsInfo.FindByPredicate([&](const FGLSSaveTabParamsInfo& Info)
        {
            return TabName == Info.TabName;
        });

    if (Tab)
    {
        if (!bEnable && bForceCleanParams)
        {
            Tab->Params.Empty();
        }

        Tab->bEnableAutoSaveParams = bEnable;
    }

    //SaveDataBinary();
}

bool UGLSSaveSubsystem::GetEnableSaveData(const EWorldType::Type Type, const int32 Instance, const FString& TabName)
{
    FGLSSaveTabContext& TabContext = GetTabContextRef(Type, Instance);
    auto Tab = TabContext.TabParamsInfo.FindByPredicate([&](const FGLSSaveTabParamsInfo& Info)
        {
            return TabName == Info.TabName;
        });

    if (Tab)
    {
        return Tab->bEnableAutoSaveParams;
    }

    return false;
}

TArray<FString> UGLSSaveSubsystem::GetTabNames(const EWorldType::Type Type, const int32 Instance)
{
    TArray<FString> TabNames;
    FGLSSaveTabContext& TabContext = GetTabContextRef(Type, Instance);
    for (const auto& TabInfo : TabContext.TabParamsInfo)
    {
        if (!TabInfo.TabName.IsEmpty())
        {
            TabNames.Add(TabInfo.TabName);
        }
    }

    return TabNames;
}

FString UGLSSaveSubsystem::GetSelectedTabName(const EWorldType::Type Type, const int32 Instance)
{
    const FGLSSaveTabContext& TabContext = GetTabContextRef(Type, Instance);
    const FGLSSaveTabParamsInfo* TabInfo = TabContext.TabParamsInfo.FindByPredicate([](const FGLSSaveTabParamsInfo& Info)
        {
            return Info.bSelected;
        });

    return TabInfo ? TabInfo->TabName : "";
}

void UGLSSaveSubsystem::SetSelectedTabByName(const EWorldType::Type Type, const int32 Instance, const FString& TabName, const bool bSelected)
{
    FGLSSaveTabContext& TabContext = GetTabContextRef(Type, Instance);
    FGLSSaveTabParamsInfo* PrevSelectedTabInfo = TabContext.TabParamsInfo.FindByPredicate([](const FGLSSaveTabParamsInfo& Info)
        {
            return Info.bSelected;
        });

    //NOTE: set selected false to prev tab (unselect)
    if (PrevSelectedTabInfo)
    {
        PrevSelectedTabInfo->bSelected = false;
    }

    //NOTE: set selected true to new tab
    FGLSSaveTabParamsInfo& TabInfo = GetTabParamsInfoRef(TabName, TabContext);
    TabInfo.bSelected = true;

    //SaveDataBinary();
}

bool UGLSSaveSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return FGLSUtils::IsGLSEnabled();
}

FGLSSaveTabParamsInfo& UGLSSaveSubsystem::GetTabParamsInfoRef(const EWorldType::Type Type, const int32 Instance, const FString& TabName)
{
    return GetTabParamsInfoRef(TabName, GetTabContextRef(Type, Instance));
}

FGLSSaveTabParamsInfo& UGLSSaveSubsystem::GetTabParamsInfoRef(const FString& TabName, FGLSSaveTabContext& TabContext)
{
    ensure(!TabName.IsEmpty());

    FGLSSaveTabParamsInfo* TabInfo = TabContext.TabParamsInfo.FindByPredicate([&](const FGLSSaveTabParamsInfo& Info)
        {
            return Info.TabName == TabName;
        });

    if (TabInfo)
    {
        return *TabInfo;
    }
    else
    {
        return TabContext.TabParamsInfo.Add_GetRef(FGLSSaveTabParamsInfo(TabName));
    }
}

FGLSSaveTabContext& UGLSSaveSubsystem::GetTabContextRef(const EWorldType::Type Type, const int32 Instance)
{
    FGLSSaveTabContext* TabContext = SaveData.TabContexts.FindByPredicate([&](const FGLSSaveTabContext& TabContext)
        {
            return TabContext.WorldType == (uint8)Type && TabContext.PIEInstance == Instance;
        });

    if (TabContext)
    {
        return *TabContext;
    }
    else
    {
        return SaveData.TabContexts.Add_GetRef(FGLSSaveTabContext((uint8)Type, Instance));
    }
}

static FString GetSaveFileNamePath()
{
    static const int32 ActualSaveDataSize = sizeof(FGLSSaveData);
    static const FString SaveDirectory = FPaths::ProjectSavedDir() + TEXT("/GameLogsSave/");
    static const FString FileName = FString::Format(TEXT("gls-save_data-{0}.bin"), {ActualSaveDataSize});
    static const FString AbsoluteFilePath = SaveDirectory + FileName;

    return AbsoluteFilePath;
}

void UGLSSaveSubsystem::SaveDataBinary()
{
    if (auto Settings = UGLSSettings::Get())
    {
        if (!Settings->bEnableLayoutSave)
        {
            return;
        }
    }

    if (SaveData.TabContexts.IsEmpty())
        return;

    static const FString AbsoluteFilePath = GetSaveFileNamePath();
    //UE_LOG(GLSSave, Display, TEXT("Save data binary to: %s"), *AbsoluteFilePath);

    TArray<uint8> OutBytes;
    FMemoryWriter MemoryReader(OutBytes, true);
    UGLSSaveSystemObject* SaveObject = NewObject<UGLSSaveSystemObject>();
    SaveObject->SaveData = SaveData;
    SaveObject->Serialize(MemoryReader);

    //For debug
    //FString JsonLogs;
    //FJsonObjectConverter::UStructToJsonObjectString(SaveData, JsonLogs);
    //
    //UE_LOG(GLSSave, Display, TEXT("JsonLogs: %s"), *JsonLogs);

    if (!FFileHelper::SaveArrayToFile(OutBytes, *AbsoluteFilePath))
    {
        //UE_LOG(GLSSave, Error, TEXT("SaveArrayToFile failure"));
    }
}

void UGLSSaveSubsystem::LoadSaveDataBinary()
{
    if (bSaveDataLoaded)
    {
        return;
    }

    if (auto Settings = UGLSSettings::Get())
    {
        if (!Settings->bEnableLayoutSave)
        {
            return;
        }
    }

    static const FString AbsoluteFilePath = GetSaveFileNamePath();
    if (!FPaths::FileExists(AbsoluteFilePath))
    {
        //GLS_LOG(this, GLSSave, Error, TEXT("File does not exist: %s."), *AbsoluteFilePath);
        return;
    }

    //GLS_LOG(this, GLSSave, Display, TEXT("Start load save from: %s"), *AbsoluteFilePath);

    TArray<uint8> OutBytes;
    if (!FFileHelper::LoadFileToArray(OutBytes, *AbsoluteFilePath))
    {
        //GLS_LOG_MESSAGE(this, GLSSave, Error, TEXT("LoadFileToArray failure"));
        return;
    }

    FMemoryReader MemoryReader(OutBytes, true);
    UGLSSaveSystemObject* SaveObject = NewObject<UGLSSaveSystemObject>();
    SaveObject->Serialize(MemoryReader);

    SaveData = MoveTemp(SaveObject->SaveData);

    bSaveDataLoaded = true;

    //For debug
    //FString JsonLogs;
    //FJsonObjectConverter::UStructToJsonObjectString(SaveData, JsonLogs);
    //
    //UE_LOG(GLSSave, Display, TEXT("JsonLogs: %s"), *JsonLogs);
}
