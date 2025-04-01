// Copyright (c) 2024 Fergius Engineering.

#include "GLSCustomCategoryModel.h"

UGLSCustomCategoryModel::UGLSCustomCategoryModel()
    : Super()
{
}

void UGLSCustomCategoryModel::SetActiveEditableMode(const EGLSCustomCetegoryEditableModeType ModeType, const bool bEnabled)
{
    if (ensure(ActiveEditableMode.Contains(ModeType)))
    {
        ActiveEditableMode[ModeType] = bEnabled;
    }
}

void UGLSCustomCategoryModel::SetActiveEditableModes(const TMap<EGLSCustomCetegoryEditableModeType, bool>& Modes)
{
    if (ensure(!Modes.IsEmpty()))
    {
        ActiveEditableMode = Modes;
    }
}

bool UGLSCustomCategoryModel::GetActiveEditableMode(const EGLSCustomCetegoryEditableModeType ModeType) const
{
    if (ensure(ActiveEditableMode.Contains(ModeType)))
    {
        return ActiveEditableMode[ModeType];
    }

    return false;
}

void UGLSCustomCategoryModel::SetEnableEditableMode(const EGLSCustomCetegoryEditableModeType ModeType, const bool bEnabled)
{
    if (ensure(EnabledEditableMode.Contains(ModeType)))
    {
        if (EnabledEditableMode[ModeType] != bEnabled)
        {
            EnabledEditableMode[ModeType] = bEnabled;
            OnEnabledEditableMode.Broadcast(this, ModeType, bEnabled);
        }
    }
}

bool UGLSCustomCategoryModel::GetEnableEditableMode(const EGLSCustomCetegoryEditableModeType ModeType) const
{
    if (ensure(EnabledEditableMode.Contains(ModeType)))
    {
        return EnabledEditableMode[ModeType];
    }

    return false;
}

void UGLSCustomCategoryModel::ChangeCategoryEnable()
{
    SetCategoryEnable(!GetCategoryEnable());
}
