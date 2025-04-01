// Copyright (c) 2024 Fergius Engineering.

#include "GLSTagsWidgetBase.h"

#include "GLSCustomCategoryModel.h"
#include "GLSSettings.h"
#include "GLSSubsystem.h"
#include "GLSWidgetBase.h"

UGLSTagsWidgetBase::UGLSTagsWidgetBase()
    : Super()
{
    CategoryTpe = EGLSCustomCategoriesTypes::Tags;
}

void UGLSTagsWidgetBase::ApplyFiltersByModel(const UGLSCustomCategoryModel* Model)
{
    if (IsValid(MainLogsWidget))
    {
        MainLogsWidget->ApplyFilterByTag(Model->GetCategoryName(), Model->GetCategoryEnable());
    }
}

void UGLSTagsWidgetBase::EnableCategoryTextOnLogs(const bool bEnable)
{
    if (IsValid(MainLogsWidget))
    {
        MainLogsWidget->SetLogsWithTags(bEnable);
    }
}
