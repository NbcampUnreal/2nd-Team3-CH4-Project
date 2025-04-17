#include "ChatMessageWidget.h"

#include "Components/TextBlock.h"

void UChatMessageWidget::SetMessageText(const FText& InText)
{
	if (TextBlock_Message)
	{
		TextBlock_Message->SetText(InText);
	}
}
