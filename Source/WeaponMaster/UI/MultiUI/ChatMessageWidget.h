#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatMessageWidget.generated.h"

UCLASS()
class WEAPONMASTER_API UChatMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetMessageText(const FText& InText);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Message;
};
