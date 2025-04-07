#include "WeaponMaster/GameModes/SingleGameMode.h"

void ASingleGameMode::BeginPlay()
{
    Super::BeginPlay();

   // if (PlayerCharacterClass && WeaponClass)
   // {
        //플레이어 캐릭터 스폰
        //몬스터 스폰
	//}
}

void ASingleGameMode::NotifyCharacterDeath(AActor* DeadActor)
{
	/*if //(Cast<AWeaponMasterCharacter>(DeadActor))
	{
		// 플레이어가 죽음
		OnPlayerLose();
	}
	else //if (Cast<ABossCharacter>(DeadActor))
	{
		// 몬스터 죽음
		OnPlayerWin();
	}*/
}

void ASingleGameMode::OnPlayerWin()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Win!"));

	// 예: UI 띄우기, 컨트롤러 입력 막기, 재시작 버튼 보여주기
	// 또는 UMG로 승리 화면 표시
}

void ASingleGameMode::OnPlayerLose()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Lose!"));

	// 마찬가지로 패배 UI 처리
}
