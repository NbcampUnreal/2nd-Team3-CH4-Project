// Copyright (c) 2024 Fergius Engineering.

#include "GLSTypes.h"

#include <Engine/Engine.h>
#include <GameFramework/PlayerController.h>
#include <GameFramework/PlayerState.h>
#include <Kismet/GameplayStatics.h>

#include "GLSSettings.h"
#include "GLSUtils.h"

DECLARE_CYCLE_STAT(TEXT("GLS LogInfo AddLogAdd"), STAT_GLSLogInfoAddLogData, STATGROUP_GLS);

FGLSLogInfo::FGLSLogInfo(UObject* Context)
{
    this->ID = FGuid::NewGuid();
    this->Time = FDateTime::Now();

    if (const auto Settings = UGLSSettings::Get())
    {
        if (IsValid(Context) && Context->IsValidLowLevelFast())
        {
            ClassName = *Context->GetClass()->GetName();
            ObjectName = *GetNameSafe(Context);

            // PIE:1, PIE:2 etc
            if (Settings->DefaultLoggingTagTypes & StaticCast<uint8>(EGLSTagsDefaultTypes::Pie) && FGLSUtils::GetPIECount() > 1)
            {
                Tags.Add(*FString::Printf(TEXT("PIE:%d"), FGLSUtils::GetPIEInstance(Context)));
            }

            // Role:Listen Server, Role:Client etc
            if (Settings->DefaultLoggingTagTypes & StaticCast<uint8>(EGLSTagsDefaultTypes::Role))
            {
                Tags.Add(*FString::Printf(TEXT("ROLE:%s"), *FGLSUtils::GetRoleByContext(Context)));
            }

            // PLAYER name
            if (Settings->DefaultLoggingTagTypes & StaticCast<uint8>(EGLSTagsDefaultTypes::PlayerName))
            {
                if (UWorld* World = GEngine->GetWorldFromContextObject(Context, EGetWorldErrorMode::ReturnNull))
                {
                    APlayerController* PC = UGameplayStatics::GetPlayerController(Context, 0);
                    if (PC && PC->PlayerState)
                    {
                        Tags.Add(*FString::Printf(TEXT("PLAYER:%s"), *PC->PlayerState->GetPlayerName()));
                    }
                }
            }
        }
    }
}

FGLSLogInfo::FGLSLogInfo(const FDateTime& InTime, const int32 Frame, const FName InCategory, const EGLSLogType InVerbosity, const FString& LogStr)
{
    this->ID = FGuid::NewGuid();
    this->Time = InTime;
    this->FrameNumber = Frame;
    this->Category = InCategory;
    this->Verbosity = InVerbosity;
    this->LogText = LogStr;
}

FGLSLogInfo::FGLSLogInfo(const FString& LogStr)
{
    this->ID = FGuid::NewGuid();
    this->LogText = LogStr;
}

TSharedPtr<FGLSLogInfo>& FLogsFullData::AddLogData(TUniquePtr<FGLSLogInfo>&& NewLog, bool& OutCategoryRecieved, bool& OutVerboseRecieved, bool& OutClassNameRecieved, bool& OutObjectNameRecieved, bool& OutTagsRecieved, TArray<FName>& OutNewTags)
{
    SCOPE_CYCLE_COUNTER(STAT_GLSLogInfoAddLogData);
    OutCategoryRecieved = OutVerboseRecieved = OutClassNameRecieved = OutObjectNameRecieved = OutTagsRecieved = false;
    auto& LogRef = RecievedLogs.Emplace_GetRef(TSharedPtr<FGLSLogInfo>(NewLog.Release()));

    //NOTE: override Verbosity level for text from blueprint
    if (LogRef->Category == FName(TEXT("LogBlueprintUserMessages")))
    {
        if (LogRef->LogText.Contains(TEXT("Warning:")))
        {
            LogRef->Verbosity = EGLSLogType::Warning;
        }
        else if (LogRef->LogText.Contains(TEXT("Error:")))
        {
            LogRef->Verbosity = EGLSLogType::Error;
        }
    }

    if (LogRef->IsContextValid())
    {
        if (!ReceivedCategories.Contains(LogRef->Category) && !LogRef->Category.IsNone())
        {
            ReceivedCategories.Add(LogRef->Category);
            OutCategoryRecieved = true;
        }

        if (!ReceivedLogTypes.Contains(LogRef->Verbosity))
        {
            ReceivedLogTypes.Add(LogRef->Verbosity);
            OutVerboseRecieved = true;
        }

        if (!LogRef->GetClassName().IsNone() && !ReceivedClassNames.Contains(LogRef->GetClassName()))
        {
            ReceivedClassNames.Add(LogRef->GetClassName());
            OutClassNameRecieved = true;
        }

        if (!LogRef->GetObjectName().IsNone() && !ReceivedObjectNames.Contains(LogRef->GetObjectName()))
        {
            ReceivedObjectNames.Add(LogRef->GetObjectName());
            OutObjectNameRecieved = true;
        }
    }

    for (const auto& Tag : LogRef->Tags)
    {
        if (!ReceivedLogTags.Contains(Tag))
        {
            ReceivedLogTags.Add(Tag);
            OutTagsRecieved = true;
            OutNewTags.Add(Tag);
        }
    }

    return LogRef;
}