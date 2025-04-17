#include "ChatWidget.h"
#include "Components/EditableTextBox.h"

void UChatWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ChatEditableTextBox)
    {
        ChatEditableTextBox->OnTextCommitted.AddDynamic(this, &UChatWidget::OnChatTextCommitted);
    }
}

void UChatWidget::OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter && !Text.IsEmpty())
    {
        OnChatMessageCommitted.Broadcast(Text);
        ChatEditableTextBox->SetText(FText::GetEmpty());
        SetVisibility(ESlateVisibility::Hidden);
    }
}