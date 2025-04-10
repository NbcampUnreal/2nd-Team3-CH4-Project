#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Instance/WeaponMasterGameInstance.h"
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
	
	// EOS 서버에서 보내주는 정보
	UFUNCTION(Client, Reliable)
	void Client_UpdateTotalPlayerNum(int16 PlayerNum);

protected:
	UFUNCTION(Server, Reliable)
	void Server_RegisterPlayer(APlayerController* PlayerController);
	
	UFUNCTION(Server, Reliable)
	void Server_StartSession();

private:
	UPROPERTY()
	int32 TimerCountDown = 100;
	
	UFUNCTION()
	void HandleProcessResult(EPlayerEOSStateType State, ESessionResultType Result);

	UFUNCTION()
	virtual void OnNetCleanup(class UNetConnection* Connection) override;
	
	UFUNCTION()
	void OnStartSessionButtonClicked();

	UFUNCTION()
	void OnLoginButtonClicked();

	UPROPERTY()
	FTimerHandle HUDTimerHandle;

	UFUNCTION()
	void HandleTimerAction();

	UPROPERTY()
	FTimerHandle PlayCountDownTimerHandle;

	UFUNCTION()
	void PlayCountDownTimerAction();

	UFUNCTION()
	void AddDelegate();

	UFUNCTION()
	void OnCooperateButtonClicked();

	UFUNCTION()
	void OnDeathMatchButtonClicked();

	UFUNCTION()
	void SetTimer();
};
