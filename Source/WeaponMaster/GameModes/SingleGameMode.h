#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SingleGameMode.generated.h"

UCLASS()
class WEAPONMASTER_API ASingleGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	// 캐릭터 생성(선택한 무기에 맞춰서)
	// AI캐릭터 생성(무기 랜덤)
	// 게임오버 처리?
	virtual void BeginPlay()override;

	void NotifyCharacterDeath(AActor* DeadActor);
	void OnPlayerWin();
	void OnPlayerLose();
};