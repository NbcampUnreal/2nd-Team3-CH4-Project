// Copyright (c) 2024 Fergius Engineering.

#include "GLSObjectNamesWidgetBase.h"

#include "GLSCustomCategoryModel.h"
#include "GLSSettings.h"
#include "GLSSubsystem.h"
#include "GLSWidgetBase.h"

 UGLSObjectNamesWidgetBase::UGLSObjectNamesWidgetBase()
    : Super()
{
     CategoryTpe = EGLSCustomCategoriesTypes::Objects;
 }

void UGLSObjectNamesWidgetBase::ApplyFiltersByModel(const UGLSCustomCategoryModel* Model)
{
    if (IsValid(MainLogsWidget))
    {
        MainLogsWidget->ApplyFilterByObjectName(Model->GetCategoryName(), Model->GetCategoryEnable());
    }
}

void UGLSObjectNamesWidgetBase::EnableCategoryTextOnLogs(const bool bEnable)
{
    if (IsValid(MainLogsWidget))
    {
        MainLogsWidget->SetLogsWithObjectNames(bEnable);
    }
}
