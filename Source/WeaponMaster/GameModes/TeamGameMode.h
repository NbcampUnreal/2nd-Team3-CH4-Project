#pragma once

#include "CoreMinimal.h"
#include "BattleGMInterface.h"
#include "GameFramework/GameMode.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "TeamGameMode.generated.h"

UCLASS()
class WEAPONMASTER_API ATeamGameMode : public AGameMode, public IBattleGMInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// BattleGMInterface
	virtual void SetPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, FName ItemName, AController* OwnerController) override;
	virtual void HandlePlayerDeath(AController* Controller);
	
	UFUNCTION(BlueprintCallable, Category = "Instance")
	FORCEINLINE_DEBUGGABLE UWeaponMasterGameInstance* GetInstance() const
	{
		return Cast<UWeaponMasterGameInstance>(GetGameInstance());
	}
	
private:
	bool HasCharacterSpawner() const;

	UPROPERTY()
	int32 TimerCountDown = 300;

	UFUNCTION()
	void SetTimer();

	UPROPERTY()
	FTimerHandle PlayCountDownTimerHandle;

	UFUNCTION()
	void PlayCountDownTimerAction();
};
