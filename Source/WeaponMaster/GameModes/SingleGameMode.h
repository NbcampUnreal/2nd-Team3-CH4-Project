#pragma once

#include "CoreMinimal.h"
#include "BattleGMInterface.h"
#include "GameFramework/GameModeBase.h"
#include "Instance/WeaponMasterGameInstance.h"
#include "SingleGameMode.generated.h"

UCLASS()
class WEAPONMASTER_API ASingleGameMode : public AGameModeBase, public IBattleGMInterface
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay()override;

	void NotifyCharacterDeath(AActor* DeadActor);
	void OnPlayerWin();
	void OnPlayerLose();

	// BattleGMInterface

	virtual void RestartPlayer(AController* NewPlayer) override;
	
	UFUNCTION(BlueprintCallable, Category = "Game")
	void UpdatePlayerInfo();
	
	virtual void SetPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, FName ItemName, AController* OwnerController) override;
	virtual void HandlePlayerDeath(AController* Controller);

	UFUNCTION(BlueprintCallable, Category = "Instance")
	FORCEINLINE_DEBUGGABLE UWeaponMasterGameInstance* GetInstance() const
	{
		return Cast<UWeaponMasterGameInstance>(GetGameInstance());
	}
private:
	bool HasCharacterSpawner() const;
};