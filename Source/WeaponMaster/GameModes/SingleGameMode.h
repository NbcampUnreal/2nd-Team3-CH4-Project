#pragma once

#include "CoreMinimal.h"
#include "BaseGameMode.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "SingleGameMode.generated.h"

UCLASS()
class WEAPONMASTER_API ASingleGameMode : public ABaseGameMode
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void NotifyCharacterDeath(AActor* DeadActor);
	void OnPlayerWin();
	void OnPlayerLose();

	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual void SpawnPlayerCharacter(APlayerController* Controller) override;
	
	UFUNCTION(BlueprintCallable, Category = "Game")
	void UpdatePlayerInfo();

	UFUNCTION(BlueprintCallable, Category = "Instance")
	FORCEINLINE_DEBUGGABLE UWeaponMasterGameInstance* GetInstance() const
	{
		return Cast<UWeaponMasterGameInstance>(GetGameInstance());
	}

};