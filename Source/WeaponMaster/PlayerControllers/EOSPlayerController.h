#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "UI/MultiUI/PlayerNameWidget.h"
#include "EOSPlayerController.generated.h"

class USessionLobbyWidget;
enum class EGameModeType : uint8;
class FOnlineSessionSearch;
class FOnlineSessionSearchResult;

UCLASS()
class WEAPONMASTER_API AEOSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEOSPlayerController();
	virtual void BeginPlay() override;

protected:
	UFUNCTION(Server, Reliable)
	void Server_RegisterPlayer(APlayerController* PlayerController);
	
	UFUNCTION(Server, Reliable)
	void Server_StartSession();

private:
	UFUNCTION()
	void HandleProcessResult(EPlayerEOSStateType State, ESessionResultType Result);

	UFUNCTION()
	virtual void OnNetCleanup(class UNetConnection* Connection) override;
	
	UFUNCTION()
	void OnStartSessionButtonClicked();

	UFUNCTION()
	void OnLoginButtonClicked();

	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SessionLobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<USessionLobbyWidget> SessionLobbyWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PlayerNameWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayerNameWidget> PlayerNameWidget;
};
