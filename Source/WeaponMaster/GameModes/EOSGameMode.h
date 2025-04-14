#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "Session/EOSGameSession.h"
#include "EOSGameMode.generated.h"

UENUM(BlueprintType)
enum class EGameModeType: uint8
{
	PVP UMETA(DisplayName = "PVP"),
	PVE UMETA(DisplayName = "PVE")
};

USTRUCT(BlueprintType)
struct FSinglePlayerController
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<APlayerController> Controller;

	bool operator==(const FSinglePlayerController& Other) const
	{
		return Controller.Get() == Other.Controller.Get();
	}
};

FORCEINLINE uint32 GetTypeHash(const FSinglePlayerController& Item)
{
	return ::GetTypeHash(Item.Controller.Get());
}

USTRUCT(BlueprintType)
struct FPlayerControllerCollection
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<FSinglePlayerController> Controller;
};

UCLASS()
class WEAPONMASTER_API AEOSGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AEOSGameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void Logout(AController* Exiting) override;
	
	UFUNCTION(BlueprintCallable, Category = "EOS")
	void TravelMap();

	UFUNCTION(BlueprintCallable, Category = "EOS")
	void StartSession();

	UFUNCTION(BlueprintCallable, Category = "EOS")
	int32 GetPlayerNum() const;

	UFUNCTION(BlueprintCallable, Category = "Session")
	FORCEINLINE_DEBUGGABLE AEOSGameSession* GetEOSGameSession() const { return Cast<AEOSGameSession>(GameSession); }

	UFUNCTION(BlueprintCallable, Category = "Instance")
	FORCEINLINE_DEBUGGABLE UWeaponMasterGameInstance* GetInstance() const
	{
		return Cast<UWeaponMasterGameInstance>(GetGameInstance());
	}
	
	UFUNCTION()
	void RegisterPlayer(APlayerController* NewPlayer);
	
private:
	UPROPERTY()
	FString MapURL = "";

	UPROPERTY()
	int32 TimerCountDown = 100;

	UFUNCTION()
	void SetTimer();

	UPROPERTY()
	FTimerHandle PlayCountDownTimerHandle;

	UFUNCTION()
	void PlayCountDownTimerAction();
	
	UFUNCTION()
	void HandleProcessResult(ESessionStateType State, ESessionResultType Result);
};
