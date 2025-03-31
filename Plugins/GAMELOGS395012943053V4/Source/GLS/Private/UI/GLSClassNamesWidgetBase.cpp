// Copyright (c) 2024 Fergius Engineering.

#include "GLSClassNamesWidgetBase.h"

#include "GLSCustomCategoryModel.h"
#include "GLSSettings.h"
#include "GLSSubsystem.h"
#include "GLSWidgetBase.h"

UGLSClassNamesWidgetBase::UGLSClassNamesWidgetBase()
    : Super()
{
    CategoryTpe = EGLSCustomCategoriesTypes::Classes;
}

void UGLSClassNamesWidgetBase::ApplyFiltersByModel(const UGLSCustomCategoryModel* Model)
{
    if (IsValid(MainLogsWidget))
    {
        MainLogsWidget->ApplyFilterByClassName(Model->GetCategoryName(), Model->GetCategoryEnable());
    }
}

void UGLSClassNamesWidgetBase::EnableCategoryTextOnLogs(const bool bEnable)
{
    if (IsValid(MainLogsWidget))
    {
        MainLogsWidget->SetLogsWithClassNames(bEnable);
    }
}
