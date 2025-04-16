
#include "WeaponMaster/GameModes/SingleGameMode.h"
// #include "EngineUtils.h"
#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "CharacterSpawner/CharacterSpawner.h"
// #include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/WeaponMasterController.h"
#include "UI/CommonUI/PlayerStatusWidget.h"
#include "UI/SingleUI/SingleGameHUD.h"
// #include "PlayerState/WeaponMasterPlayerState.h"
#include "Characters/Components/ItemComponent/ItemComponent.h"

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

void ASingleGameMode::SpawnPlayerCharacter(APlayerController* Controller)
{
	Super::SpawnPlayerCharacter(Controller);

	UE_LOG(LogTemp, Warning, TEXT("ASingleGameMode::SpawnPlayerCharacter : Called."));
	
	if (!HasCharacterSpawner()) return;
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterSpawner::StaticClass(), FoundActors);

	uint32 Cnt = 50000;

	while (--Cnt)
	{
		uint8 RandomSpawnerIndex = FMath::RandRange(0, FoundActors.Num() - 1);
		ACharacterSpawner* CharacterSpawner = Cast<ACharacterSpawner>(FoundActors[RandomSpawnerIndex]);

		if (const UWeaponMasterGameInstance* WMGI = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
		{
			if (ACharacter* SpawnCharacter = CharacterSpawner->SpawnCharacter(WMGI->CharacterClass))
			{
				if (AWeaponMasterController* WMPC = Cast<AWeaponMasterController>(Controller))
				{
					// Possess할때 Owner설정도 됨!
					// SpawnCharacter->SetOwner(WMPC);
					WMPC->Possess(SpawnCharacter);
					UE_LOG(LogTemp, Warning, TEXT("Possessed Pawn: %s"), *WMPC->GetPawn()->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SetPlayerCharacter : Possess Failed."));
				}
				
				if (SpawnCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
				{
					UItemComponent* ItemComponent = IBattleSystemUser::Execute_GetItemComponent(SpawnCharacter);
					ItemComponent->EquipItem(WMGI->ItemName);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ATeamGameMode::SetPlayerCharacter : SpawnCharacter doesn't implement IBattleSystemUser."));
				}
			
				break;
			}
		}
	}
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

	// 플레이어 이름 가져오기 - GameInstance 사용
	FString PlayerName = GetPlayerNameFromGameInstance(PC);
    
	// 플레이어 상태 정보 구성
	FPlayerStatusInfo StatusInfo;
	StatusInfo.PlayerName = PlayerName;
	StatusInfo.CurrentHealth = Character->GetHP();
	StatusInfo.MaxHealth = Character->GetMaxHP();
	StatusInfo.PlayerThumbnailTexture = Character->GetCharacterThumbnail();
	StatusInfo.CharacterID = 0; // 싱글 플레이어는 ID 0
	StatusInfo.TeamID = 0;      // 싱글 플레이어는 팀 0

    
	// HUD 업데이트
	SingleHUD->UpdatePlayerStatus(StatusInfo);
	UE_LOG(LogTemp, Display, TEXT("Player info updated with name: %s"), *PlayerName);
}
