// Copyright (c) 2024 Fergius Engineering.

#include "GLSLogItemBase.h"

#include <Styling/AppStyle.h>
#include <Styling/CoreStyle.h>
#include <Styling/SlateStyleMacros.h>
#include <Styling/SlateTypes.h>
#include <Styling/StyleColors.h>

#include "Common/GLSSettings.h"
#include "UI/GLSMultiLineEditableText.h"

void UGLSLogItemBase::SetLogSettings(const TSharedPtr<FGLSLogSettings>& InLogSettingsPtr)
{
    LogSettingsPtr = InLogSettingsPtr;
    SetupText();
}

void UGLSLogItemBase::SetHighlightText(const TSharedPtr<FText>& InHighlightTextPtr)
{
    HighlightTextPtr = InHighlightTextPtr;
    EditableTextLog->SetHighlightText(FText::GetEmpty());
    SetupHighlightText();
}

void UGLSLogItemBase::SetupHighlightText()
{
    if (HighlightTextPtr && IsValid(EditableTextLog))
    {
        if (HighlightTextPtr->ToString() != EditableTextLog->GetSearchText())
        {
            EditableTextLog->SetHighlightText(*HighlightTextPtr);
        }
    }
}

void UGLSLogItemBase::SetupText()
{
    if (IsValid(LogModelPtr) && IsValid(EditableTextLog))
    {
        const FString FormattedString = LogModelPtr->GetFormattedString(LogSettingsPtr);
        if (FormattedString != EditableTextLog->GetText().ToString())
        {
            EditableTextLog->SetText(FText::FromString(FormattedString));
        }
    }
}

void UGLSLogItemBase::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    EditableTextLog->SetText(FText::GetEmpty());
    EditableTextLog->SetHighlightText(FText::GetEmpty());

    LogModelPtr = Cast<UGLSLogInfoObject>(ListItemObject);
    InitializeLogItem();
    SetupText();
    SetupHighlightText();
}

void UGLSLogItemBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    SetupText();
    SetupHighlightText();
}

void UGLSLogItemBase::NativePreConstruct()
{
    Super::NativePreConstruct();

    FTextBlockStyle NormalText = FAppStyle::GetWidgetStyle<FTextBlockStyle>("NormalText");

    int32 FontSize = 9;
    if (auto Settings = UGLSSettings::Get())
    {
        FontSize = Settings->LogsFontSize;
    }

    const FTextBlockStyle NormalLogText = FTextBlockStyle(NormalText)
                                              .SetFont(DEFAULT_FONT("Mono", FontSize))
                                              .SetColorAndOpacity(FStyleColors::Foreground)
                                              .SetSelectedBackgroundColor(FStyleColors::Highlight)
                                              .SetHighlightColor(FStyleColors::Black);

    EditableTextLog->SetWidgetStyle(NormalLogText);
}