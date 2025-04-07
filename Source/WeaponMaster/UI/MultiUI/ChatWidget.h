// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

class UEditableTextBox;

UCLASS()
class WEAPONMASTER_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

protected:

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* ChatEditableTextBox;

    UFUNCTION()
    void OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};
