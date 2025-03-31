// Copyright (c) 2024 Fergius Engineering.

#include "GLSListView.h"

#include <Components/ListViewBase.h>

#include "GLSLogInfoObject.h"

void UGLSListView::AddLogItem(const class UGLSLogInfoObject* Model)
{
    if (Model == nullptr)
    {
        return;
    }

    UObject* Item = (UObject*)Model;

    ListItems.Add(Item);

    const TArray<UObject*> Added = {Item};
    const TArray<UObject*> Removed;
    OnItemsChanged(Added, Removed);

    RequestRefresh();
}

UUserWidget& UGLSListView::OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
    UUserWidget& CreatedEntryWidget = Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
    OnGeneratedEntryWidget.Broadcast(Item, &CreatedEntryWidget);
    return CreatedEntryWidget;
}