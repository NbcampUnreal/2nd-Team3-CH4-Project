// Copyright (c) 2024 Fergius Engineering.

#include "GLSUtils.h"

#if WITH_EDITOR
#include "Editor.h"
#include "ISettingsModule.h"
#endif

#include <CoreGlobals.h>
#include <Engine/Engine.h>
#include <Engine/World.h>
#include <HAL/PlatformApplicationMisc.h>
#include <Internationalization/Regex.h>
#include <Misc/ConfigCacheIni.h>
#include <Modules/ModuleManager.h>

#include "GLSSettings.h"
#include "GLSSubsystem.h"

int32 FGLSUtils::GetPIEInstance(UObject* Context)
{
    if (!IsValid(Context) || !Context->IsValidLowLevelFast())
    {
        return INDEX_NONE;
    }

#if WITH_EDITOR
    const FWorldContext* _WorldContext = GEngine->GetWorldContextFromWorld(Context->GetWorld());
    if (_WorldContext && _WorldContext->PIEInstance != INDEX_NONE)
    {
        return _WorldContext->PIEInstance;
    }
#endif
    return INDEX_NONE;
}

int32 FGLSUtils::GetPIECount()
{
    int32 _PIECount = 0;
#if WITH_EDITOR
    if (GEditor)
    {
        const auto& _WorldContexts = GEditor->GetWorldContexts();
        for (const FWorldContext& _WorldContext : _WorldContexts)
        {
            if (_WorldContext.WorldType == EWorldType::PIE)
            {
                _PIECount++;
            }
        }
    }
#endif
    return _PIECount;
}

FString FGLSUtils::GetTagsStrByLogInfo(const TSharedPtr<FGLSLogInfo>& LogInfoPtr)
{
    FString TagsStr;
    if (LogInfoPtr.IsValid())
    {
        for (int32 i = 0; i < LogInfoPtr->Tags.Num(); i++)
        {
            const FString Separator = (i < LogInfoPtr->Tags.Num() - 1 ? ", " : "");
            const FString Tag = "[" + LogInfoPtr->Tags[i].ToString() + "]" + Separator;
            TagsStr.Append(Tag);
        }
    }

    return TagsStr;
}

FString FGLSUtils::GetFormattedStringByLogDataAndSettings(const TSharedRef<FGLSLogInfo>& LogInfoRef, const TSharedPtr<FGLSLogSettings>& LogSettingsPtr, const int32 DuplicatedCount)
{
    const auto GetDuplicateStr = [&LogSettingsPtr, &LogInfoRef, &DuplicatedCount]()
    {
        return LogSettingsPtr && !LogSettingsPtr->bLogsDuplicates && DuplicatedCount > 1 ? FString::Printf(TEXT("[X%d]"), DuplicatedCount) : FString("");
    };

    const auto GetTimeStr = [&LogSettingsPtr, &LogInfoRef]()
    {
        return LogSettingsPtr && LogSettingsPtr->bLogsWithTime ? FString::Printf(TEXT("[%s]"), *LogInfoRef->GetTime().ToString(TEXT("%Y.%m.%d-%H.%M.%S:%s"))) : FString("");
    };

    const auto GetFrameStr = [&LogSettingsPtr, &LogInfoRef]()
    {
        return LogSettingsPtr && LogSettingsPtr->bLogsWithFrame ? FString::Printf(TEXT("[%3i]"), LogInfoRef->FrameNumber % 1000) : FString("");
    };

    const auto GetCategoryStr = [&LogSettingsPtr, &LogInfoRef]()
    {
        return LogSettingsPtr && LogSettingsPtr->bLogsWithCategory ? FString::Printf(TEXT("%s:"), *LogInfoRef->Category.ToString()) : FString("");
    };

    const auto GetVerbosityStr = [&LogSettingsPtr, &LogInfoRef]()
    {
        return LogSettingsPtr && LogSettingsPtr->bLogsWithVerbosity ? FString::Printf(TEXT(" %s:"), LogInfoRef->GetVerbosityStr()) : FString("");
    };

    const auto GetTagsStr = [&LogSettingsPtr, &LogInfoRef]()
    {
        return LogSettingsPtr && LogSettingsPtr->bLogsWithTags && !LogInfoRef->Tags.IsEmpty() ? FString::Printf(TEXT(" %s:"), *GetTagsStrByLogInfo(LogInfoRef)) : FString("");
    };

    const auto GetClassNameStr = [&LogSettingsPtr, &LogInfoRef]()
    {
        return LogSettingsPtr && LogSettingsPtr->bLogsWithClassName && LogInfoRef->GetClassName().IsValid() ? FString::Printf(TEXT(" %s:"), *LogInfoRef->GetClassName().ToString()) : FString("");
    };

    const auto GetObjectNameStr = [&LogSettingsPtr, &LogInfoRef]()
    {
        return LogSettingsPtr && LogSettingsPtr->bLogsWithObjectName && LogInfoRef->GetClassName().IsValid() ? FString::Printf(TEXT(" %s:"), *LogInfoRef->GetObjectName().ToString()) : FString("");
    };

    FString Result = FString::Printf(TEXT("%s%s%s%s%s%s%s%s %s"),
        *GetDuplicateStr(),
        *GetTimeStr(),
        *GetFrameStr(),
        *GetCategoryStr(),
        *GetVerbosityStr(),
        *GetTagsStr(),
        *GetClassNameStr(),
        *GetObjectNameStr(),
        *LogInfoRef->LogText);

    if (!Result.IsEmpty())
    {
        if (Result[0] == ' ')
        {
            Result.RemoveAt(0);
        }
    }

    return Result;
}

FString FGLSUtils::GetRoleByContext(UObject* Context)
{
    FString RoleStr;

    if (GEngine && IsValid(Context) && Context->IsValidLowLevelFast())
    {
        UWorld* World = GEngine->GetWorldFromContextObject(Context, EGetWorldErrorMode::ReturnNull);
        FString Prefix;
        if (World)
        {
            FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(World);
            switch (World->GetNetMode())
            {
            case NM_Client:
#if WITH_EDITOR
                RoleStr = WorldContext ? FString::Printf(TEXT("Client %d"), WorldContext->PIEInstance) : "";
                break;
#else
                RoleStr = TEXT("Client");
                break;
#endif
            case NM_DedicatedServer:
                RoleStr = TEXT("Dedicated Server");
                break;
            case NM_ListenServer:
                RoleStr = TEXT("Listen Server");
                break;
            case NM_Standalone:
                RoleStr = TEXT("Standalone");
                break;
            }
        }
    }

    return RoleStr;
}

void FGLSUtils::CopyLogsToClipboard(const FString& Logs)
{
    FPlatformApplicationMisc::ClipboardCopy(*Logs);
}

TSharedPtr<FGLSLogInfo> FGLSUtils::ParseLogEnty(const FString& LogEnty)
{
    const TArray<FString> Patterns = {
        TEXT(R"(\[(\d{4}\.\d{2}\.\d{2}-\d{2}\.\d{2}\.\d{2}:\d{3})\]\[\s*(\d+)\s*\]\s*([\w\s]+):\s*([\w\s]+):\s*(.+))"),
        TEXT(R"(([\w\s]+):\s*([\w\s]+):\s*(.+))"),
        TEXT(R"(([\w\s]+):\s*(.+))"),
        TEXT(R"(.+)")};

    FDateTime Time;
    int32 Frame = 0;
    FName Category;
    EGLSLogType Verbose = EGLSLogType::NoLogging;
    FString LogStr;
    bool bParsed = false;

    for (const FString& Pattern : Patterns)
    {
        FRegexPattern RegexPattern(Pattern);
        FRegexMatcher RegexMatcher(RegexPattern, LogEnty);

        if (RegexMatcher.FindNext())
        {
            if (Pattern.Contains(TEXT("\\d{4}\\.")))
            {
                const FString TimeString = RegexMatcher.GetCaptureGroup(1);
                FDateTime::Parse(*TimeString, Time);
            }

            if (Pattern.Contains(TEXT("\\s*(\\d+)\\s*")))
            {
                const FString FrameString = RegexMatcher.GetCaptureGroup(2);
                Frame = FCString::Atoi(*FrameString);
            }

            if (Pattern.Contains(TEXT("([\\w\\s]+):")))
            {
                Category = FName(*RegexMatcher.GetCaptureGroup(3));
            }

            if (Pattern.Contains(TEXT("([\\w\\s]+):")))
            {
                Verbose = (EGLSLogType)ParseLogVerbosityFromString(*RegexMatcher.GetCaptureGroup(4));
            }

            LogStr = RegexMatcher.GetCaptureGroup(5);

            if (LogStr.IsEmpty())
            {
                LogStr = LogEnty;
            }

            bParsed = true;
            break;
        }
    }

    if (!bParsed)
    {
        UE_LOG(LogTemp, Log, TEXT("INVALID : %s"), *LogEnty);
        return MakeShared<FGLSLogInfo>(LogEnty);
    }

    return MakeShared<FGLSLogInfo>(Time, Frame, Category, Verbose, LogStr);
}

bool FGLSUtils::IsGLSEnabled()
{
#if UE_SERVER
    return false;
#endif

#if !defined(GLS_ENABLED) || !GLS_ENABLED
    return false;
#endif

    if (auto Settings = UGLSSettings::Get())
    {
#if WITH_EDITOR
        return Settings->bEnableLogsInEditor;
#elif UE_BUILD_DEVELOPMENT
        return Settings->bEnableLogsInDevelopmentBuilds;
#elif UE_BUILD_DEBUG
        return Settings->bEnableLogsInDebugBuilds;
#elif UE_BUILD_SHIPPING
        return Settings->bEnableLogsInShippingBuilds;
#elif USE_LOGGING_IN_SHIPPING
        return Settings->bEnableLoggingInShippingWithLogs;
#elif UE_BUILD_TEST
        return Settings->bEnableLogsInTestBuilds;
#else
        return true;
#endif
    }

    return false;
}

bool UGLSUtils::IsDesktopPlatform()
{
    return PLATFORM_DESKTOP;
}

bool UGLSUtils::IsConsolePlatform()
{
    //TODO: need add XB1
#if (defined(PLATFORM_PS4) && PLATFORM_PS4) || (defined(PLATFORM_PS5) && PLATFORM_PS5) || (defined(PLATFORM_XBOXONE) && PLATFORM_XBOXONE) || (defined(PLATFORM_XSX) && PLATFORM_XSX) || (defined(PLATFORM_SWITCH) && PLATFORM_SWITCH)
    return true;
#else
    return false;
#endif
}

bool UGLSUtils::IsMobilePlatform()
{
#if PLATFORM_ANDROID || PLATFORM_IOS
    return true;
#else
    return false;
#endif
}

bool UGLSUtils::IsEditor()
{
    return WITH_EDITOR;
}

bool UGLSUtils::IsEditorContext(UObject* Context)
{
    if (!IsValid(Context) || !Context->IsValidLowLevelFast())
    {
        return false;
    }

    if (auto World = Context->GetWorld())
    {
        return World->WorldType == EWorldType::Editor || World->WorldType == EWorldType::EditorPreview;
    }

    return false;
}

void UGLSUtils::OpenGLSEditorSettings()
{
#if WITH_EDITOR
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->ShowViewer("Project", "Plugins", "Game logs");
    }
#endif
}

int32 UGLSUtils::GetMaxObjectsInGame()
{
    static const int32 MaxObjects = []()
    {
        int32 Value = 0;
#if WITH_EDITOR
        GConfig->GetInt(TEXT("/Script/Engine.GarbageCollectionSettings"), TEXT("gc.MaxObjectsInEditor"), Value, GEngineIni);
#elif IS_PROGRAM
        GConfig->GetInt(TEXT("/Script/Engine.GarbageCollectionSettings"), TEXT("gc.MaxObjectsInProgram"), Value, GEngineIni);
#else
        GConfig->GetInt(TEXT("/Script/Engine.GarbageCollectionSettings"), TEXT("gc.MaxObjectsInGame"), Value, GEngineIni);
#endif
        return Value;
    }();

    return MaxObjects;
}

bool UGLSUtils::IsLogsEmpty()
{
    return UGLSUtils::GetLogsNum() == 0;
}

const int32 UGLSUtils::GetLogsNum()
{
    if (!FGLSUtils::IsGLSEnabled())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        return 0;
    }

    return UGLSSubsystem::Get()->GetLogsFullData().RecievedLogs.Num();
}

const TArray<TSharedPtr<FGLSLogInfo>>& UGLSUtils::GetLogs()
{
    if (!FGLSUtils::IsGLSEnabled())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        static TArray<TSharedPtr<FGLSLogInfo>> Default;
        return Default;
    }

    return UGLSSubsystem::Get()->GetLogsFullData().RecievedLogs;
}

const TSet<FName>& UGLSUtils::GetReceivedCategories(UObject* Context)
{
    if (!FGLSUtils::IsGLSEnabled() || !IsValid(Context) || !Context->IsValidLowLevel())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        static TSet<FName> Default;
        return Default;
    }

    return UGLSSubsystem::Get()->GetLogsFullData().ReceivedCategories;
}

const TSet<FName>& UGLSUtils::GetReceivedClassNames(UObject* Context)
{
    if (!FGLSUtils::IsGLSEnabled() || !IsValid(Context) || !Context->IsValidLowLevel())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        static TSet<FName> Default;
        return Default;
    }

    return UGLSSubsystem::Get()->GetLogsFullData().ReceivedClassNames;
}

const TSet<FName>& UGLSUtils::GetReceivedObjectNames(UObject* Context)
{
    if (!FGLSUtils::IsGLSEnabled() || !IsValid(Context) || !Context->IsValidLowLevel())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        static TSet<FName> Default;
        return Default;
    }

    return UGLSSubsystem::Get()->GetLogsFullData().ReceivedObjectNames;
}

const TSet<FName>& UGLSUtils::GetReceivedTags(UObject* Context)
{
    if (!FGLSUtils::IsGLSEnabled() || !IsValid(Context) || !Context->IsValidLowLevel())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        static const TSet<FName> Default;
        return Default;
    }

    return UGLSSubsystem::Get()->GetLogsFullData().ReceivedLogTags;
}

const FLogsFullData& UGLSUtils::GetLogsFullData(UObject* Context)
{
    if (!FGLSUtils::IsGLSEnabled() || !IsValid(Context) || !Context->IsValidLowLevel())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        static const FLogsFullData Default;
        return Default;
    }

    return UGLSSubsystem::Get()->GetLogsFullData();
}

FOnLogCategoryReceivedDelegate& UGLSUtils::GetCategoryRecievedEvent(UObject* Context)
{
    if (!FGLSUtils::IsGLSEnabled() || !IsValid(Context) || !Context->IsValidLowLevel())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        static FOnLogCategoryReceivedDelegate DefaultDeleate;
        return DefaultDeleate;
    }

    return UGLSSubsystem::Get()->OnCategoryRecieved;
}

FOnLogClassNameReceivedDelegate& UGLSUtils::GetClassNameRecievedEvent(UObject* Context)
{
    if (!FGLSUtils::IsGLSEnabled() || !IsValid(Context) || !Context->IsValidLowLevel())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        static FOnLogClassNameReceivedDelegate DefaultDeleate;
        return DefaultDeleate;
    }

    return UGLSSubsystem::Get()->OnClassNameRecieved;
}

FOnLogObjectNameReceivedDelegate& UGLSUtils::GetObjectNameRecievedEvent(UObject* Context)
{
    if (!FGLSUtils::IsGLSEnabled() || !IsValid(Context) || !Context->IsValidLowLevel())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        static FOnLogObjectNameReceivedDelegate DefaultDeleate;
        return DefaultDeleate;
    }

    return UGLSSubsystem::Get()->OnObjectNameRecieved;
}

FOnLogTagsReceivedDelegate& UGLSUtils::GetTagsRecievedEvent(UObject* Context)
{
    if (!FGLSUtils::IsGLSEnabled() || !IsValid(Context) || !Context->IsValidLowLevel())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        static FOnLogTagsReceivedDelegate DefaultDeleate;
        return DefaultDeleate;
    }

    return UGLSSubsystem::Get()->OnTagsRecieved;
}

FOnLogVerboseReceivedDelegate& UGLSUtils::GetVerboseRecievedEvent(UObject* Context)
{
    if (!FGLSUtils::IsGLSEnabled() || !IsValid(Context) || !Context->IsValidLowLevel())
    {
        //NOTE: for case if GLS disabled but UI editor was created
        static FOnLogVerboseReceivedDelegate DefaultDeleate;
        return DefaultDeleate;
    }

    return UGLSSubsystem::Get()->OnVerboseRecieved;
}