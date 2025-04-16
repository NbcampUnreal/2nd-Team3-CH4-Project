#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillLogEntryWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class WEAPONMASTER_API UKillLogEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void Setup(const FText& KillerName, const FText& VictimName) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KillLog")
	UTexture2D* DefaultKillIcon;

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillerText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkullIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* VictimText;

private:
	FTimerHandle AutoRemoveTimer;

	UFUNCTION()
	void HandleAutoRemove();
};
