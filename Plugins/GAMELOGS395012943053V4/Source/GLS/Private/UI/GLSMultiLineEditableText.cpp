// Copyright (c) 2024 Fergius Engineering.

#include "GLSMultiLineEditableText.h"

#include "Widgets/Text/SMultiLineEditableText.h"

void UGLSMultiLineEditableText::SetHighlightText(const FText& InSearchText)
{
    if (MyMultiLineEditableText.IsValid())
    {
        MyMultiLineEditableText->GoTo(FTextLocation(0));
        MyMultiLineEditableText->ClearSelection();
        MyMultiLineEditableText->BeginSearch(InSearchText);
    }
}

FString UGLSMultiLineEditableText::GetSearchText() const
{
    if (MyMultiLineEditableText.IsValid())
    {
        return MyMultiLineEditableText->GetSearchText().ToString();
    }

    return "";
}
