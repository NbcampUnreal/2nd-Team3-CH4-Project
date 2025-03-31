// Copyright (c) 2024 Fergius Engineering.

#include "GLSLogInfoObject.h"

#include "GLSUtils.h"

FString UGLSLogInfoObject::GetFormattedString(const TSharedPtr<FGLSLogSettings>& SettingsPtr) const
{
    if (LogInfoPtr.IsValid())
    {
        return FGLSUtils::GetFormattedStringByLogDataAndSettings(LogInfoPtr.ToSharedRef(), SettingsPtr, DiplicateCount);
    }

    return TEXT("");
}
