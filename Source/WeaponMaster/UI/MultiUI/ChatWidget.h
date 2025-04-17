#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

class UEditableTextBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChatMessageCommitted, const FText&, Message);

UCLASS()
class WEAPONMASTER_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

    UPROPERTY(BlueprintAssignable, Category = "Chat")
    FOnChatMessageCommitted OnChatMessageCommitted;

protected:

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* ChatEditableTextBox;

    UFUNCTION()
    void OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};
