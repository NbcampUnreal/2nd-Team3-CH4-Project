#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "EOSPlayerController.generated.h"

class USessionLobbyWidget;
enum class EGameModeType : uint8;
class FOnlineSessionSearch;
class FOnlineSessionSearchResult;

// HUD & EOS & RPC
UENUM(BlueprintType)
enum class EMapType : uint8
{
	PVPMap     UMETA(DisplayName = "PVPMap"),
	PVEMap        UMETA(DisplayName = "PVEMap"),
	SessionMap     UMETA(DisplayName = "SessionMap")
};

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

	// 세션 타이머
	UFUNCTION(Client, Reliable)
	void Client_UpdateTimer(const int32 TimerCountDown);

	// 인게임 타이머
	UFUNCTION(Client, Reliable)
	void Client_UpdateInGameTimer(const int32 TimerCountDown);
	
	// 플레이어 사망정보
	UFUNCTION(Client, Reliable)
	void Client_PlayerDead(const FString& Killer, const FString& Victim);
	
	// 플레이어 등록
	UFUNCTION(Client, Reliable)
	void Client_UpdatePlayers();

	UFUNCTION()
	void UpdateHUD(EMapType Map);
	
	UFUNCTION()
	void SetSelectedPlayerWidget();
	
protected:
	UFUNCTION(Server, Reliable)
	void Server_RegisterPlayer(APlayerController* PlayerController);
	
	UFUNCTION(Server, Reliable)
	void Server_StartSession();

	UFUNCTION(Server, Reliable)
	void Server_SetCooperationMapSelected(bool IsVoted);

	UFUNCTION(Server, Reliable)
	void Server_SetDeathMatchMapSelected(bool IsVoted);

private:
	UPROPERTY()
	EMapType CurrentMap;
	
	UPROPERTY()
	bool bIsVoted = false;

	UPROPERTY()
	bool bIsCooperateVoted = false;

	UPROPERTY()
	bool bIsDeathMatchVoted = false;
	
	UFUNCTION()
	void HandleProcessResult(EPlayerEOSStateType State, ESessionResultType Result);

	UFUNCTION()
	virtual void OnNetCleanup(class UNetConnection* Connection) override;

	UFUNCTION()
	void Login();

	UPROPERTY()
	FTimerHandle HUDTimerHandle;

	UFUNCTION()
	void HandleTimerAction();

	UPROPERTY()
	FTimerHandle PlayerStatusTimerHandle;

	UFUNCTION()
	void SetPlayerStatus();

	UFUNCTION()
	void AddDelegate();

	UFUNCTION()
	void OnCooperateButtonClicked();

	UFUNCTION()
	void OnDeathMatchButtonClicked();

	UFUNCTION()
	void SetTimer();
};
