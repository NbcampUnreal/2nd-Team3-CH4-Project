#include "WeaponMaster/GameModes/SingleGameMode.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "CharacterSpawner/CharacterSpawner.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/ItemComponent/ItemComponent.h"
#include "GameFramework/PlayerState.h"
#include "UI/CommonUI/PlayerStatusWidget.h"
#include "UI/SingleUI/SingleGameHUD.h"

void ASingleGameMode::BeginPlay()
{
    Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("===== 멀티플레이어 테스트 시작 ====="));
   // if (PlayerCharacterClass && WeaponClass)
   // {
        //플레이어 캐릭터 스폰
        //몬스터 스폰
	//}

	// 플레이어 정보 초기화
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ASingleGameMode::UpdatePlayerInfo);
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

void ASingleGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ASingleGameMode::UpdatePlayerInfo);
}

void ASingleGameMode::UpdatePlayerInfo()
{
	// 플레이어 컨트롤러 가져오기
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}
    
	// 플레이어 캐릭터 가져오기
	ABaseBattleCharacter* Character = Cast<ABaseBattleCharacter>(PC->GetPawn());
	if (!Character)
	{
		return;
	}
    
	// HUD 가져오기
	ASingleGameHUD* SingleHUD = Cast<ASingleGameHUD>(PC->GetHUD());
	if (!SingleHUD)
	{
		return;
	}
    
	// 플레이어 상태 정보 구성
	FPlayerStatusInfo StatusInfo;
	StatusInfo.PlayerName = PC->PlayerState ? PC->PlayerState->GetPlayerName() : FString("Player");
	StatusInfo.CurrentHealth = Character->GetHP();
	StatusInfo.MaxHealth = Character->GetMaxHP();
	StatusInfo.PlayerThumbnailTexture = Character->GetCharacterThumbnail();
	StatusInfo.CharacterID = 0; // 싱글 플레이어는 ID 0
	StatusInfo.TeamID = 0;      // 싱글 플레이어는 팀 0

    
	// HUD 업데이트
	SingleHUD->UpdatePlayerStatus(StatusInfo);
}

bool ASingleGameMode::HasCharacterSpawner() const
{
	for (TActorIterator<ACharacterSpawner> It(GetWorld()); It; ++It)
	{
		return true; 
	}
	return false;
}

void ASingleGameMode::SetPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, FName ItemName, AController* OwnerController)
{
	if (!HasCharacterSpawner()) return;
	bool bSuccessFlag = false;
	while (true)
	{
		for (TActorIterator<ACharacterSpawner> It(GetWorld()); It; ++It)
		{
			ACharacterSpawner* CharacterSpawner = *It;
			
			if (ACharacter* SpawnCharacter = CharacterSpawner->SpawnCharacter(CharacterClass))
			{
				bSuccessFlag = true;

				SpawnCharacter->SetOwner(OwnerController);
				OwnerController->Possess(SpawnCharacter);

				if (SpawnCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
				{
					UItemComponent* ItemComponent = IBattleSystemUser::Execute_GetItemComponent(SpawnCharacter);
					ItemComponent->EquipItem(ItemName);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SetPlayerCharacter : SpawnCharacter doesn't implement IBattleSystemUser."));
				}
				
				break;
			}
		}
		if (bSuccessFlag) break;
	}
}
