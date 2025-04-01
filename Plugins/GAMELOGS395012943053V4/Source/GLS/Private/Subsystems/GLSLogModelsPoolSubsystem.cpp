// Copyright (c) 2024 Fergius Engineering.

#include "GLSLogModelsPoolSubsystem.h"

#include <Engine/Engine.h>
#include <Misc/MessageDialog.h>

#include "GLSLogInfoObject.h"
#include "GLSSubsystem.h"
#include "GLSTypes.h"
#include "GLSUtils.h"

UGLSLogModelsPoolSubsystem* UGLSLogModelsPoolSubsystem::Get(const UObject* WorldContext)
{
    if (!IsInGameThread())
    {
        return nullptr;
    }

    if (!IsValid(WorldContext) || !WorldContext->IsValidLowLevelFast())
    {
        return nullptr;
    }

    return GEngine->GetEngineSubsystem<UGLSLogModelsPoolSubsystem>();
}

UGLSLogInfoObject* UGLSLogModelsPoolSubsystem::CreateModel(const TSharedPtr<FGLSLogInfo>& Info, bool& bPoolCleared)
{
    bPoolCleared = false;

    static bool bMessageDisplayed = false;
    if (!bMessageDisplayed && MaxObjectsInGame > 0 && LogsModelPool.Num() >= (MaxObjectsInGame - 5000)) //report problems in advance, not just at the last second
    {
        const EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::OkCancel, EAppReturnType::Ok, FText::FromString(FString::Printf(TEXT("Maximum number of UObjects (%d / %d) exceeded, make sure you update MaxObjectsInGame/MaxObjectsInEditor/MaxObjectsInProgram. The overflow will soon cause the program to crash. Clear all Game Logs System UObjects to prevent crash?"), LogsModelPool.Num(), MaxObjectsInGame)));
        if (ReturnType == EAppReturnType::Ok)
        {
            bPoolCleared = true;
            bMessageDisplayed = false;

            for (auto& ModelPair : LogsModelPool)
            {
                if (IsValid(ModelPair.Value) && ModelPair.Value->IsValidLowLevelFast(false))
                {
                    ModelPair.Value->MarkAsGarbage();
                    ModelPair.Value->ConditionalBeginDestroy();
                }
            }

            LogsModelPool.Empty(MaxObjectsInGame);

            CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

            if (auto Subsystem = UGLSSubsystem::Get())
            {
                Subsystem->ClearAllLogsData();
            }

            OnModelsCleared.Broadcast();
        }
        else
        {
            bMessageDisplayed = true;
        }
    }

    if (auto ModelPtr = LogsModelPool.Find(Info->GetID()))
    {
        return ModelPtr->Get();
    }

    UGLSLogInfoObject* LogInfoObject = NewObject<UGLSLogInfoObject>(this, NAME_None, RF_Transient);
    LogInfoObject->InitializeLogObject(Info);

    LogsModelPool.Add(Info->GetID(), LogInfoObject);

    return LogInfoObject;
}

void UGLSLogModelsPoolSubsystem::ResetModels()
{
    LogsModelPool.Empty(LogsModelPool.Num());
}

bool UGLSLogModelsPoolSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return FGLSUtils::IsGLSEnabled();
}

void UGLSLogModelsPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    MaxObjectsInGame = UGLSUtils::GetMaxObjectsInGame();
}
