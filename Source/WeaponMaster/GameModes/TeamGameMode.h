#pragma once

#include "CoreMinimal.h"
#include "BaseGameMode.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "TeamGameMode.generated.h"

UCLASS()
class WEAPONMASTER_API ATeamGameMode : public ABaseGameMode
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Instance")
	FORCEINLINE_DEBUGGABLE UWeaponMasterGameInstance* GetInstance() const
	{
		return Cast<UWeaponMasterGameInstance>(GetGameInstance());
	}

	virtual void SpawnPlayerCharacter(APlayerController* Controller) override;
	

	UFUNCTION()
	void BroadcastGameResultsToClients(int32 Results);

	void OnBossDefeated();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TArray<TSubclassOf<ACharacter>> CharacterClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TArray<FName> ItemNames;
	
	UPROPERTY()
	int32 TimerCountDown = 30;

	UFUNCTION()
	void SetTimer();

	UPROPERTY()
	FTimerHandle PlayCountDownTimerHandle;

	UFUNCTION()
	void PlayCountDownTimerAction();
};
