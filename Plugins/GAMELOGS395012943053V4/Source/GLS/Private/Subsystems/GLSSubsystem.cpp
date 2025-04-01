// Copyright (c) 2024 Fergius Engineering.

#include "GLSSubsystem.h"

#include <Kismet/GameplayStatics.h>

#if WITH_EDITOR
#include <Editor.h>
#endif

#include "GLSLogModelsPoolSubsystem.h"
#include "GLSMacroses.h"
#include "GLSUtils.h"

DECLARE_CYCLE_STAT(TEXT("GLS Subsystem ProcessNewLogEntries"), STAT_GLSSubsystemProcessNewLogEntries, STATGROUP_GLS);
DECLARE_CYCLE_STAT(TEXT("GLS Subsystem ForeProcessNewLogEntries"), STAT_ForceProcessNewLogEntries, STATGROUP_GLS);
DECLARE_CYCLE_STAT(TEXT("GLS Subsystem AddLog_Internal"), STAT_GLSSubsystemAddLog_Internal, STATGROUP_GLS);

namespace GLSSubsystemHelper
{
    UWorld* GetWorldStatic()
    {
        UWorld* World = nullptr;
        if (GEngine)
        {
            for (const FWorldContext& Context : GEngine->GetWorldContexts())
            {
                if (Context.WorldType == EWorldType::Game ||
                    Context.WorldType == EWorldType::GamePreview ||
                    Context.WorldType == EWorldType::PIE)
                {
                    World = Context.World();
                }
            }
        }
        return World;
    }
} // namespace GLSSubsystemHelper

void UGLSSubsystem::PrintStringToGLS(UObject* WorldContextObject, const FString& InString /*= FString(TEXT("Hello"))*/, const FString& Category /*= FString(TEXT("GLS"))*/, const EGLSLogType Verbosity /*= EGLSLogType::Display,*/, bool bPrintToScreen /*= true*/, bool bPrintToLog /*= true*/, FLinearColor TextColor /*= FLinearColor(0.0, 0.66, 1.0)*/, float Duration /*= 2.f*/, const FName Key /*= NAME_None*/)
{
    UGLSSubsystem::PrintStringToGLSWithTags(WorldContextObject, {}, InString, Category, Verbosity, bPrintToScreen, bPrintToLog, TextColor, Duration, Key);
}

void UGLSSubsystem::PrintStringToGLSWithTags(UObject* WorldContextObject, const TArray<FString>& Tags, const FString& InString /*= FString(TEXT("Hello"))*/, const FString& Category /*= FString(TEXT("GLS"))*/, const EGLSLogType Verbosity /*= EGLSLogType::Display*/, bool bPrintToScreen /*= true*/, bool bPrintToLog /*= true*/, FLinearColor TextColor /*= FLinearColor(0.0, 0.66, 1.0)*/, float Duration /*= 2.f*/, const FName Key /*= NAME_None*/)
{
    if (IsValid(WorldContextObject) && WorldContextObject->IsValidLowLevelFast())
    {
        if (FGLSUtils::IsGLSEnabled())
        {
            if (auto Subsystem = UGLSSubsystem::Get())
            {
                if (auto OutputDevice = FGLSOutputDevice::Get())
                {
                    OutputDevice->SerializeForGLS(WorldContextObject, Tags, InString, (ELogVerbosity::Type)Verbosity, Category, bPrintToLog);
                }
            }
        }

        UKismetSystemLibrary::PrintString(WorldContextObject, InString, bPrintToScreen, false, TextColor, Duration, Key);
    }

    #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) || USE_LOGGING_IN_SHIPPING
    if (bPrintToLog && GLog)
    {
        GLog->Serialize(*InString, (ELogVerbosity::Type)Verbosity, *Category);
    }
#endif
}

UGLSSubsystem* UGLSSubsystem::Get()
{
    if (IsInGameThread() && !IsEngineExitRequested())
    {
        return GEngine->GetEngineSubsystem<UGLSSubsystem>();
    }

    return nullptr;
}

void UGLSSubsystem::ClearAllLogsData()
{
    LogsFullData.RecievedLogs = {};
}

bool UGLSSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return FGLSUtils::IsGLSEnabled();
}

void UGLSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

#if WITH_EDITOR
    FEditorDelegates::PreBeginPIE.AddUObject(this, &UGLSSubsystem::OnBeginPIE);
#endif

    LogsFullData.RecievedLogs.Reserve(20480);
    LogsFullData.ReceivedCategories.Reserve(256);
    LogsFullData.ReceivedClassNames.Reserve(256);
    LogsFullData.ReceivedObjectNames.Reserve(256);
    LogsFullData.ReceivedLogTags.Reserve(256);

    if (auto OutputDevice = FGLSOutputDevice::Get())
    {
        const auto Settings = UGLSSettings::Get();
        const int32 LimitPerFrame = Settings ? Settings->MaxLogsPerFrame : 1024;
        ProcessNewLogEntries(OutputDevice->ExtractReceivedLogs(LimitPerFrame));
    }
}

void UGLSSubsystem::Deinitialize()
{
    Super::Deinitialize();

    FGLSOutputDevice* OutputDevice = FGLSOutputDevice::Get();
    if (GLog && OutputDevice)
    {
        GLog->RemoveOutputDevice(OutputDevice);
    }
}

void UGLSSubsystem::Tick(float DeltaTime)
{
    ForceProcessNewLogEntries();
}

void UGLSSubsystem::OnBeginPIE(bool bSimulate)
{
    LastBeginPIETime = FDateTime::Now();
    OnPIEStarted.Broadcast();
}

void UGLSSubsystem::AddLog_Internal(TUniquePtr<FGLSLogInfo>&& LogInfo)
{
    SCOPE_CYCLE_COUNTER(STAT_GLSSubsystemAddLog_Internal);
    if (LogInfo->Verbosity == EGLSLogType::NoLogging)
        return;

    bool CategoryRecieved, VerboseRecieved, ClassNameRecieved, ObjectNameRecieved, TagsRecieved;
    TArray<FName> NewTags;

    const TSharedPtr<FGLSLogInfo>& NewLogInfo = LogsFullData.AddLogData(MoveTemp(LogInfo), CategoryRecieved, VerboseRecieved, ClassNameRecieved, ObjectNameRecieved, TagsRecieved, NewTags);
    // !!! DO NOT ACCESS LogInfo AFTER THIS LINE

    if (CategoryRecieved)
    {
        OnCategoryRecieved.Broadcast(NewLogInfo->Category);
    }

    if (VerboseRecieved)
    {
        OnVerboseRecieved.Broadcast(NewLogInfo->Verbosity);
    }

    if (ClassNameRecieved)
    {
        OnClassNameRecieved.Broadcast(NewLogInfo->GetClassName());
    }

    if (ObjectNameRecieved)
    {
        OnObjectNameRecieved.Broadcast(NewLogInfo->GetObjectName());
    }

    if (TagsRecieved)
    {
        OnTagsRecieved.Broadcast(NewTags);
    }

    OnLogRecieved.Broadcast(NewLogInfo);
}

void UGLSSubsystem::ProcessNewLogEntries(TArray<TUniquePtr<FGLSLogInfo>>&& NewEntries)
{
    SCOPE_CYCLE_COUNTER(STAT_GLSSubsystemProcessNewLogEntries);
    for (auto& Entry : NewEntries)
    {
        AddLog_Internal(MoveTemp(Entry));
    }
}

void UGLSSubsystem::ForceProcessNewLogEntries()
{
    SCOPE_CYCLE_COUNTER(STAT_ForceProcessNewLogEntries);
    if (auto OutputDevice = FGLSOutputDevice::Get())
    {
        auto Settings = UGLSSettings::Get();
        const int32 LimitPerFrame = Settings ? Settings->MaxLogsPerFrame : 1024;

        ProcessNewLogEntries(OutputDevice->ExtractReceivedLogs(LimitPerFrame));
    }
}
