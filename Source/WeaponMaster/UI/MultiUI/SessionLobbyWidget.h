#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionLobbyWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartSessionClicked);

class UButton;

UCLASS()
class WEAPONMASTER_API USessionLobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	FOnStartSessionClicked OnStartSessionClicked;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartSessionButton;

protected:
	UFUNCTION()
	void HandleStartSessionButtonClicked();
};
