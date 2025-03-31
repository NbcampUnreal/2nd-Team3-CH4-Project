// Copyright (c) 2024 Fergius Engineering.

#include "GLSGategoryTextItemWidget.h"

#include <Components/Button.h>
#include <Components/TextBlock.h>
#include <Engine/Engine.h>

#include "GLSCustomCategoryModel.h"

FName UGLSGategoryTextItemWidget::GetCategoryName() const
{
    if (IsValid(ModelObject))
    {
        return ModelObject->GetCategoryName();
    }

    return NAME_None;
}

bool UGLSGategoryTextItemWidget::GetGategoryButtonEnabled() const
{
    return IsValid(ModelObject) && ModelObject->GetCategoryEnable();
}

void UGLSGategoryTextItemWidget::NativeOnEntryReleased()
{
    ModelObject = nullptr;

    IUserObjectListEntry::NativeOnEntryReleased();
}

void UGLSGategoryTextItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    ModelObject = Cast<UGLSCustomCategoryModel>(ListItemObject);

    //const FText _CategoryName = FText::FromString(ModelObject->GetCategoryName().ToString() + " " + FString::FromInt(ModelObject->Number));
    const FText _CategoryName = FText::FromName(ModelObject->GetCategoryName());
    CategoryNameText->SetText(_CategoryName);
    CategoryNameText->SetToolTipText(_CategoryName);

    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}