// Fill out your copyright notice in the Description page of Project Settings.


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
        //Controller에서 하나 함수 파자 
        //Controller 인풋모드 게임으로 설정해야함


        ChatEditableTextBox->SetText(FText::GetEmpty());
    }
}