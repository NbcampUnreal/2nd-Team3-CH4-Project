#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillLogWidget.generated.h"

class UKillLogEntryWidget;
class UScrollBox;

UCLASS()
class WEAPONMASTER_API UKillLogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void AddKillLogEntry(const FText& Killer, const FText& Victim);

protected:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* KillLogScrollBox;

	UPROPERTY(EditDefaultsOnly, Category = "KillLog")
	TSubclassOf<UKillLogEntryWidget> KillLogEntryClass;

private:
	UFUNCTION()
	void RemoveKillLogEntry(UUserWidget* Entry);
};
