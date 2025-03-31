// Copyright (c) 2024 Fergius Engineering.

#include "GLSCategoriesWidgetBase.h"

#include <Kismet/GameplayStatics.h>
#include <Misc/AssertionMacros.h>

#include "GLSCustomCategoryModel.h"
#include "GLSListView.h"
#include "GLSSettings.h"
#include "GLSSubsystem.h"
#include "GLSWidgetBase.h"

UGLSCategoriesWidgetBase::UGLSCategoriesWidgetBase()
    : Super()
{
    CategoryTpe = EGLSCustomCategoriesTypes::Categories;
}

void UGLSCategoriesWidgetBase::ApplyFiltersByModel(const UGLSCustomCategoryModel* Model)
{
    if (IsValid(MainLogsWidget))
    {
        MainLogsWidget->ApplyFilterByCategory(Model->GetCategoryName(), Model->GetCategoryEnable());
    }
}

void UGLSCategoriesWidgetBase::EnableCategoryTextOnLogs(const bool bEnable)
{
    if (IsValid(MainLogsWidget))
    {
        MainLogsWidget->SetLogsWithCategory(bEnable);
    }
}