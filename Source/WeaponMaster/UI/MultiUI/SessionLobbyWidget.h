#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionLobbyWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartSessionClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoginClicked);

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

	FOnLoginClicked OnLoginClicked;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LoginButton;

protected:
	UFUNCTION()
	void HandleStartSessionButtonClicked();

	UFUNCTION()
	void HandleLoginButtonClicked();
};
