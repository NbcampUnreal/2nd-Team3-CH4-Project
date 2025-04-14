#include "GameModes/DeathMatchGameMode.h"
#include "CharacterSpawner/CharacterSpawner.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
#include "Characters/BaseBattleCharacter/BaseBattleCharacter.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/ItemComponent/ItemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/EOSPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "PlayerState/WeaponMasterPlayerState.h"
#include "UI/MultiUI/DeathMatchHUD.h"

ADeathMatchGameMode::ADeathMatchGameMode()
{
    // 기본 생성자
}

void ADeathMatchGameMode::BeginPlay()
{
	Super::BeginPlay();
    
	// 매치 타이머 초기화 (5분 = 300초)
	TimerCountDown = 300;
	SetTimer();
    
	// HUD 클래스 설정
	HUDClass = ADeathMatchHUD::StaticClass();
    
	UE_LOG(LogTemp, Warning, TEXT("===== 데스매치 게임 모드 시작 ====="));
}

void ADeathMatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
    
	// 데스매치는 각 플레이어를 개별 팀으로 취급
	if (AWeaponMasterPlayerState* PlayerState = Cast<AWeaponMasterPlayerState>(NewPlayer->PlayerState))
	{
		// 각 플레이어에게 고유 ID 할당 (PlayerState의 PlayerId 이용)
		int32 PlayerId = PlayerState->GetPlayerId();
		PlayerState->SetTeamID(PlayerId); // 개인전이므로 팀 ID는 플레이어 ID와 동일하게 설정
        
		// 플레이어 상태 초기화
		PlayerState->SetKillCount(0);
		PlayerState->SetDeathCount(0);
		PlayerState->SetTotalDamageDealt(0);
        
		// 플레이어 연결 로그
		UE_LOG(LogTemp, Warning, TEXT("플레이어 연결: %s (ID: %d)"), 
			   *PlayerState->GetPlayerName(), PlayerId);
	}
    
	// HUD 업데이트 - 새 플레이어 추가
	UpdateAllPlayerHUDs();
}

void ADeathMatchGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	UE_LOG(LogTemp, Warning, TEXT("플레이어 로그아웃: %s"), *Exiting->GetName());
}

bool ADeathMatchGameMode::HasCharacterSpawner() const
{
	for (TActorIterator<ACharacterSpawner> It(GetWorld()); It; ++It)
	{
		return true; 
	}
	return false;
}

void ADeathMatchGameMode::SetTimer()
{
	GetWorldTimerManager().SetTimer(
		PlayCountDownTimerHandle,
		this,
		&ADeathMatchGameMode::PlayCountDownTimerAction,
		1.0f,
		true
	);
}

void ADeathMatchGameMode::PlayCountDownTimerAction()
{
	if (TimerCountDown > 0)
	{
		--TimerCountDown;
        
		// 모든 플레이어에게 타이머 업데이트
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (AEOSPlayerController* PlayerController = Cast<AEOSPlayerController>(Iterator->Get()))
			{
				PlayerController->Client_UpdateTimer(TimerCountDown);
                
				// DeathMatchHUD 업데이트
				if (ADeathMatchHUD* DMHUD = Cast<ADeathMatchHUD>(PlayerController->GetHUD()))
				{
					DMHUD->SetRemainingTime(TimerCountDown);
				}
			}
		}
	}

	if (TimerCountDown == 0)
	{
		// 매치 종료, 승자 결정 및 스코어보드 표시
		DisplayMatchResults();
		GetWorldTimerManager().ClearTimer(PlayCountDownTimerHandle);
	}
}

void ADeathMatchGameMode::DisplayMatchResults()
{
    // 가장 높은 킬 수 찾기
    int32 HighestKills = -1;
    APlayerController* WinningPlayer = nullptr;
    
    // 모든 플레이어 컨트롤러를 순회하며 가장 높은 킬 수를 찾음
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PlayerController = Iterator->Get();
        if (!PlayerController) continue;
        
        int32 Kills = GetPlayerKills(PlayerController);
        
        UE_LOG(LogTemp, Display, TEXT("플레이어: %s, 최종 킬 수: %d"), 
            *PlayerController->GetName(), Kills);
        
        if (Kills > HighestKills)
        {
            HighestKills = Kills;
            WinningPlayer = PlayerController;
        }
    }
    
    if (WinningPlayer)
    {
        AWeaponMasterPlayerState* WinnerState = Cast<AWeaponMasterPlayerState>(WinningPlayer->PlayerState);
        FString WinnerName = WinnerState ? WinnerState->GetPlayerName() : TEXT("알 수 없음");
        
        UE_LOG(LogTemp, Warning, TEXT("매치 종료! 승자: %s (킬 수: %d)"), 
            *WinnerName, HighestKills);
            
        // 모든 플레이어에게 승자 통보
        for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
        {
            if (APlayerController* PlayerController = Cast<APlayerController>(Iterator->Get()))
            {
                // 각 플레이어의 HUD 업데이트
                ADeathMatchHUD* DMHUD = Cast<ADeathMatchHUD>(PlayerController->GetHUD());
                if (DMHUD)
                {
                    // 현재 플레이어가 승자인지 확인
                    bool bIsWinner = (PlayerController == WinningPlayer);
                    
                    // 최종 점수 계산 (킬 * 100)
                    AWeaponMasterPlayerState* PlayerState = Cast<AWeaponMasterPlayerState>(PlayerController->PlayerState);
                    int32 FinalScore = PlayerState ? PlayerState->GetKillCount() * 100 : 0;
                    
                    // 게임 종료 화면 표시
                    DMHUD->ShowGameOverScreen(bIsWinner, FinalScore);
                }
            }
        }
        
        // 게임 종료 후 로비로 돌아가는 타이머 설정
        FTimerHandle ReturnToLobbyTimerHandle;
        GetWorldTimerManager().SetTimer(
            ReturnToLobbyTimerHandle,
            [this]()
            {
                // 로비로 이동
                GetWorld()->ServerTravel("/Game/WeaponMaster/Maps/Lobby", true);
            },
            15.0f, // 15초 후 로비로 이동
            false
        );
    }
}

void ADeathMatchGameMode::SpawnPlayerCharacter(TSubclassOf<ACharacter> CharacterClass, FName ItemName, AController* OwnerController)
{
    if (!HasCharacterSpawner()) 
    {
        UE_LOG(LogTemp, Error, TEXT("캐릭터 스포너가 없습니다!"));
        return;
    }

    // 랜덤 스폰 위치 선택
    TArray<AActor*> SpawnerActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterSpawner::StaticClass(), SpawnerActors);
    
    if (SpawnerActors.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("맵에 캐릭터 스포너가 없습니다!"));
        return;
    }
    
    // 스폰 위치 랜덤 선택 및 캐릭터 스폰
    uint8 SpawnAttempts = 0;
    while (SpawnAttempts < 10) // 최대 10번 시도
    {
        int32 RandomIndex = FMath::RandRange(0, SpawnerActors.Num() - 1);
        ACharacterSpawner* Spawner = Cast<ACharacterSpawner>(SpawnerActors[RandomIndex]);
        
        if (ACharacter* SpawnedCharacter = Spawner->SpawnCharacter(CharacterClass))
        {
            // 컨트롤러 연결
            SpawnedCharacter->SetOwner(OwnerController);
            OwnerController->Possess(SpawnedCharacter);
            
            // 무기 장착
            if (SpawnedCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
            {
                UItemComponent* ItemComponent = IBattleSystemUser::Execute_GetItemComponent(SpawnedCharacter);
                ItemComponent->EquipItem(ItemName);
                
                UE_LOG(LogTemp, Display, TEXT("플레이어 캐릭터 스폰 성공: %s"), *OwnerController->GetName());
                return;
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("캐릭터가 IBattleSystemUser를 구현하지 않았습니다."));
            }
        }
        
        SpawnAttempts++;
    }
    
    UE_LOG(LogTemp, Error, TEXT("유효한 스폰 위치를 찾을 수 없습니다."));
}

void ADeathMatchGameMode::HandlePlayerDeath(AController* Controller)
{
    if (!Controller) return;
    
    // 플레이어 스테이트 가져오기
    AWeaponMasterPlayerState* VictimState = Cast<AWeaponMasterPlayerState>(Controller->PlayerState);
    if (VictimState)
    {
        // 데스 카운트 증가
        VictimState->AddDeath();
        
        // 킬러 찾기 (직접 구현 필요)
        AController* KillerController = nullptr;
        ABaseBattleCharacter* VictimCharacter = Cast<ABaseBattleCharacter>(Controller->GetPawn());
        
        if (VictimCharacter)
        {
            // 마지막 데미지를 준 컨트롤러 찾기 (구현 필요)
            KillerController = FindLastDamagingController(VictimCharacter);
            
            // 킬러가 있고, 자살이 아닌 경우 킬 점수 증가
            if (KillerController && KillerController != Controller)
            {
                AWeaponMasterPlayerState* KillerState = Cast<AWeaponMasterPlayerState>(KillerController->PlayerState);
                if (KillerState)
                {
                    KillerState->AddKill();
                    
                    // 킬 로그
                    FString KillerName = KillerState->GetPlayerName();
                    FString VictimName = VictimState->GetPlayerName();
                    
                    UE_LOG(LogTemp, Warning, TEXT("플레이어 %s가 %s를 처치! 총 킬 수: %d"), 
                        *KillerName, *VictimName, KillerState->GetKillCount());
                    
                    // HUD 업데이트
                    UpdateAllPlayerHUDs();
                }
            }
        }
        
        // 로그 기록
        FString VictimName = VictimState->GetPlayerName();
        UE_LOG(LogTemp, Warning, TEXT("플레이어 사망: %s"), *VictimName);
    }
    
    // 리스폰 타이머 설정
    FTimerHandle RespawnTimerHandle;
    GetWorldTimerManager().SetTimer(
        RespawnTimerHandle,
        [this, Controller]()
        {
            if (Controller->IsPlayerController())
            {
                RestartPlayer(Controller);
            }
        },
        RespawnDelay,
        false
    );
}


int32 ADeathMatchGameMode::GetPlayerKills(AController* Controller) const
{
	if (!Controller) return 0;
	
	if (AWeaponMasterPlayerState* PlayerState = Cast<AWeaponMasterPlayerState>(Controller->PlayerState))
	{
		return PlayerState->GetKillCount();
	}
	
	return 0;
}

void ADeathMatchGameMode::UpdateAllPlayerHUDs()
{
	// 모든 플레이어 컨트롤러 순회
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC) continue;
        
		ADeathMatchHUD* DMHUD = Cast<ADeathMatchHUD>(PC->GetHUD());
		if (!DMHUD) continue;
        
		// 모든 플레이어의 상태 정보 업데이트
		for (FConstPlayerControllerIterator PlayerIt = GetWorld()->GetPlayerControllerIterator(); PlayerIt; ++PlayerIt)
		{
			APlayerController* OtherPC = PlayerIt->Get();
			if (!OtherPC || !OtherPC->PlayerState) continue;
            
			AWeaponMasterPlayerState* PlayerState = Cast<AWeaponMasterPlayerState>(OtherPC->PlayerState);
			if (!PlayerState) continue;
            
			// 플레이어 ID는 PlayerId로 가정
			int32 PlayerId = OtherPC->GetUniqueID(); // 또는 적절한 방법으로 ID 획득
			int32 Kills = PlayerState->GetKillCount();
			int32 Deaths = PlayerState->GetDeathCount();
            
			// 플레이어 캐릭터 체력 정보
			ABaseBattleCharacter* Character = Cast<ABaseBattleCharacter>(OtherPC->GetPawn());
			float CurrentHealth = Character ? Character->GetHP() : 0.0f;
			float MaxHealth = Character ? Character->GetMaxHP() : 100.0f;
            
			// HUD 업데이트
			DMHUD->UpdatePlayerStats(PlayerId, Kills, Deaths, Kills * 100); // 점수 = 킬 * 100
			DMHUD->UpdatePlayerHealth(PlayerId, CurrentHealth, MaxHealth);
		}
	}
}


AController* ADeathMatchGameMode::FindLastDamagingController(ABaseBattleCharacter* Character)
{
	// 실제 구현은 프로젝트의 데미지 로직에 따라 달라집니다.
	// 예시로 구현:
	if (!Character) return nullptr;
    
	// BaseBattleCharacter에 LastDamageCauser와 같은 변수가 있다고 가정
	// 이 변수는 프로젝트의 데미지 처리 과정에서 설정되어야 함
	// return Character->LastDamageCauser;
    
	// 현재 제공된 코드에서는 이 정보를 가져올 방법이 없으므로
	// 데미지 처리 시스템을 추가로 구현해야 합니다.
	return nullptr;
}
