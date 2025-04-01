// Copyright (c) 2024 Fergius Engineering.

#include "GLSOutputDevice.h"

#include <Async/Async.h>
#include <CoreGlobals.h>
#include <Misc/CoreDelegates.h>
#include <Misc/OutputDeviceHelper.h>
#include <Runtime/Launch/Resources/Version.h>
#include <UObject/UObjectBase.h>

#include "GLSSettings.h"
#include "GLSSubsystem.h"

DECLARE_CYCLE_STAT(TEXT("GLS Subsystem ExtractReceivedLogs"), STAT_GLSOutputDeviceExtractReceivedLogs, STATGROUP_GLS);

bool FGLSOutputDevice::bDestructed = false;
TUniquePtr<FGLSOutputDevice> FGLSOutputDevice::OutputDevice;

FGLSOutputDevice::FGLSOutputDevice()
    : FOutputDevice()
{
    if (GLog)
    {
        GLog->AddOutputDevice(this);
    }
}

FGLSOutputDevice::~FGLSOutputDevice()
{
    bDestructed = true;
}

FGLSOutputDevice* FGLSOutputDevice::FGLSOutputDevice::Get()
{
    if (bDestructed)
        return nullptr;

    if (OutputDevice == nullptr)
    {
        OutputDevice = MakeUnique<FGLSOutputDevice>();
    }

    return OutputDevice.Get();
}

TArray<TUniquePtr<FGLSLogInfo>> FGLSOutputDevice::ExtractReceivedLogs(const int32 MaxEntries)
{
    SCOPE_CYCLE_COUNTER(STAT_GLSOutputDeviceExtractReceivedLogs);
    if (MaxEntries <= 0)
        return {};

    FScopeLock ScopeLock(&Sync);

    if (ReceivedLogs.Num() <= MaxEntries) // best case scenario
    {
        return MoveTemp(ReceivedLogs);
    }

    // move logs to the new array without invoking move constructors and destuctors
    TArray<TUniquePtr<FGLSLogInfo>> ExtractedLogs;
#if (ENGINE_MAJOR_VERSION > 5) || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 5)
    ExtractedLogs.SetNumUninitialized(MaxEntries, EAllowShrinking::No);
#else
    ExtractedLogs.SetNumUninitialized(MaxEntries, false);
#endif

    FMemory::Memcpy(ExtractedLogs.GetData(), ReceivedLogs.GetData(), MaxEntries * sizeof(TUniquePtr<FGLSLogInfo>));
    FMemory::Memzero(ReceivedLogs.GetData(), MaxEntries * sizeof(TUniquePtr<FGLSLogInfo>));

#if (ENGINE_MAJOR_VERSION > 5) || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 5)
    ReceivedLogs.RemoveAt(0, MaxEntries, EAllowShrinking::No);
#else
    ReceivedLogs.RemoveAt(0, MaxEntries, false);
#endif

    return MoveTemp(ExtractedLogs);
}

void FGLSOutputDevice::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category)
{
    FScopeLock ScopeLock(&Sync);

    if (IsEngineExitRequested())
    {
        return;
    }

    if (ExcludeEntries.Contains(V))
    {
        ExcludeEntries.Remove(V);
    }
    else
    {
        AddSharedLogInfo(nullptr, V, Verbosity, Category, {});
    }
}

void FGLSOutputDevice::SerializeForGLS(UObject* Context, const TArray<FString>& Tags, const FString& Message, ELogVerbosity::Type Verbosity, const FString& Category, bool bPrintToLog)
{
    if (Get() != nullptr)
    {
        TArray<FName> TagsName;
        for (const FString& Tag : Tags)
        {
            TagsName.Add(*Tag);
        }

        Get()->SerializeForGLS_Internal(Context, TagsName, Message, Verbosity, Category, bPrintToLog);
    }
}

void FGLSOutputDevice::SerializeForGLS(const UObject* Context, const TArray<FString>& Tags, const FString& Message, ELogVerbosity::Type Verbosity, const FString& Category, bool bPrintToLog)
{
    FGLSOutputDevice::SerializeForGLS(const_cast<UObject*>(Context), Tags, Message, Verbosity, Category, bPrintToLog);
}

void FGLSOutputDevice::AddExcludeEntry(const FString& LogEntry)
{
    FScopeLock ScopeLock(&Sync);
    ExcludeEntries.Add(LogEntry);
}

void FGLSOutputDevice::SerializeForGLS_Internal(UObject* Context, const TArray<FName>& Tags, const FString& Message, ELogVerbosity::Type Verbosity, const FString& Category, bool bPrintToLog)
{
    FScopeLock ScopeLock(&Sync);
    AddSharedLogInfo(Context, *Message, Verbosity, *Category, Tags);
}

void FGLSOutputDevice::AddSharedLogInfo(UObject* Context, const TCHAR* Text, ELogVerbosity::Type Verbosity, const FName& Category, const TArray<FName>& Tags)
{
    if (auto Settings = UGLSSettings::Get())
    {
        EGLSLogType LogVerbosity = EGLSLogType::NoLogging;
        if (Verbosity == ELogVerbosity::All)
        {
            LogVerbosity = EGLSLogType::VeryVerbose;
        }
        else if (Verbosity > ELogVerbosity::NoLogging && Verbosity < ELogVerbosity::Type::All)
        {
            LogVerbosity = StaticCast<EGLSLogType>(Verbosity);
        }
        else
        {
            //NOTE: skip other verbosities
            return;
        }

        const bool bExcludeByCategory = Settings->ExcludedLogsByCategory.ContainsByPredicate([&](const FGLSCategoryWithVerbosities& It)
            {
                const bool bCategoriesCompare = It.Category == Category;
                if (bCategoriesCompare && It.Verbosities.IsEmpty())
                    return true;

                if (bCategoriesCompare && It.Verbosities.Contains(StaticCast<EGLSLogType>(Verbosity)))
                    return true;

                return false;
            });
        if (bExcludeByCategory)
            return;

        const bool bExcludeByVerbosity = Settings->ExcludedLogsByVerbosity.Contains(StaticCast<EGLSLogType>(Verbosity));
        if (bExcludeByVerbosity)
            return;

        TUniquePtr<FGLSLogInfo> LogPtr = MakeUnique<FGLSLogInfo>(Context);
        LogPtr->Verbosity = LogVerbosity;
        LogPtr->Category = Category;
        LogPtr->LogText = Text;
        LogPtr->LogNum = ++LogsNum;
        LogPtr->FrameNumber = GFrameNumber;
        LogPtr->Tags.Append(Tags);

        FScopeLock ScopeLock(&Sync);
        ReceivedLogs.Add(MoveTemp(LogPtr));
    }
}
