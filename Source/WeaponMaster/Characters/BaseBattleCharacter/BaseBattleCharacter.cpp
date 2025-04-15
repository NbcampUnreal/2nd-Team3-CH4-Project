// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBattleCharacter.h"
#include "SSTCharacterMovementComponent.h"
#include "FollowCameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "InputActionValue.h"
#include "Characters/Components/StateComponent/StateComponent.h"
#include "Characters/Components/ItemComponent/ItemComponent.h"
#include "Characters/Components/SkillComponent/SkillComponent.h"
#include "Characters/Components/EffectComponent/EffectComponent.h"
#include "Components/CapsuleComponent.h"
#include "Skills/BaseSkill.h"
#include "Items/InteractionComponent/InteractionComponent.h"
#include "WeaponMaster/PlayerControllers/WeaponMasterController.h"
#include "Data/StatusTypes.h"
#include "GameModes/BattleGMInterface.h"
#include "UI/MultiUI/MultiGameHUD.h"
#include "Items/PickupableItem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "PlayerState/WeaponMasterPlayerState.h"
#include "UI/CommonUI/PlayerStatusWidget.h"
#include "UI/SingleUI/SingleGameHUD.h"
#include "Components/WidgetComponent.h"

// Sets default values
ABaseBattleCharacter::ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USSTCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Components
	StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("StateComponent"));
	EffectComponent = CreateDefaultSubobject<UEffectComponent>(TEXT("EffectComponent"));
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
	// WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	// WidgetComponent->SetupAttachment(GetRootComponent());
	
	// Constants
	MaxHP = 100.0f;
	HP = MaxHP;

	// CharacterMovement Constants Setting
	GetCharacterMovement()->AirControl = 1.0f;
	GetCharacterMovement()->AirControlBoostMultiplier = 0.0f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 0.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;

	// Replicate
	bReplicates = true;

	// Initialize ActorPointers
	InteractableActor = nullptr;
	LastAttacker = nullptr;
	CharacterThumbnail = nullptr;
}

// Called when the game starts or when spawned
void ABaseBattleCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(ItemComponent) && IsValid(SkillComponent))
	{
		// Item Equipment Event Binding
		ItemComponent->OnItemEquipped.AddDynamic(this, &ABaseBattleCharacter::OnItemEquippedForBinding);
		ItemComponent->OnItemUnequipped.AddDynamic(this, &ABaseBattleCharacter::OnItemUnequippedForBinding);

		SkillComponent->OnSkillStarted.AddDynamic(this, &ABaseBattleCharacter::OnSkillStarted);
		SkillComponent->OnSkillEnded.AddDynamic(this, &ABaseBattleCharacter::OnSkillEnded);
	}

	if (!HasAuthority())
	{
		SetupMontageEndedDelegate_Implementation();
	}
}

void ABaseBattleCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseBattleCharacter, HP);
	DOREPLIFETIME(ABaseBattleCharacter, EffectComponent);
}

void ABaseBattleCharacter::OnRep_HP()
{
    UE_LOG(LogTemp, Warning, TEXT("======= OnRep_HP 호출! 현재 HP: %f ======="), HP);
    
    // 플레이어 상태 정보 구성
    FPlayerStatusInfo StatusInfo;
    
    // 플레이어 이름 가져오기 - 우선 GameInstance에서 시도
    if (UWeaponMasterGameInstance* GameInstance = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
    {
        StatusInfo.PlayerName = GameInstance->GetPlayerName();
        
        // 이름이 비어있으면 PlayerState에서 가져오기
        if (StatusInfo.PlayerName.IsEmpty() && GetPlayerState())
        {
            StatusInfo.PlayerName = GetPlayerState()->GetPlayerName();
        }
    }
    else if (auto PlayerStateRef = GetPlayerState())
    {
        // GameInstance 접근 실패시 PlayerState에서 이름 가져오기
        StatusInfo.PlayerName = PlayerStateRef->GetPlayerName();
        StatusInfo.CharacterID = PlayerStateRef->GetPlayerId();
        
        // WeaponMasterPlayerState 접근 시도
        if (auto WMPS = Cast<AWeaponMasterPlayerState>(PlayerStateRef))
        {
            StatusInfo.TeamID = WMPS->GetTeamID();
        }
    }
    else
    {
        // 둘 다 실패했을 경우 기본값 설정
        StatusInfo.PlayerName = GetName();
        StatusInfo.CharacterID = 0;
    }
    
    StatusInfo.CurrentHealth = HP;
    StatusInfo.MaxHealth = MaxHP;
    StatusInfo.PlayerThumbnailTexture = GetCharacterThumbnail();
    
    // 로컬 컨트롤러가 있는 경우에만 UI 업데이트
    if (auto PlayerController = Cast<APlayerController>(GetController()))
    {
        UE_LOG(LogTemp, Warning, TEXT("HP UI 업데이트 성공! 현재 HP: %f, 최대 HP: %f, 플레이어 이름: %s"), 
               StatusInfo.CurrentHealth, StatusInfo.MaxHealth, *StatusInfo.PlayerName);
        
        // 멀티 게임 HUD 확인
        if (auto CastedHUD = Cast<AMultiGameHUD>(PlayerController->GetHUD()))
        {
            // HUD를 통해 플레이어 상태 업데이트
            CastedHUD->UpdatePlayerStatus(StatusInfo.CharacterID, StatusInfo);
        }
        // 싱글 게임 HUD 확인
        else if (auto SingleHUD = Cast<ASingleGameHUD>(PlayerController->GetHUD()))
        {
            // 싱글 게임 HUD 업데이트
            SingleHUD->UpdatePlayerStatus(StatusInfo);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ABaseBattleCharacter::OnRep_HP : HUD Cast Failed"));
        }
    }
    else
    {
        // 컨트롤러가 없는 경우 (AI 또는 리플리케이션된 캐릭터)
        UE_LOG(LogTemp, Warning, TEXT("컨트롤러가 없거나 PlayerController가 아닙니다. HP 업데이트만 처리합니다."));
    }
}

// Called every frame
void ABaseBattleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseBattleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent = PlayerInputComponent;

	if (GetLocalRole() != ROLE_SimulatedProxy)
	{
		if (StateComponent->bIsComponentReady)
		{
			BindInputFunctions();
		}
		else
		{
			StateComponent->OnStateComponentReady.BindUObject(this, &ABaseBattleCharacter::BindInputFunctions);
		}
	}
}

void ABaseBattleCharacter::BindInputFunctions()
{
	AWeaponMasterController* WeaponMasterController = Cast<AWeaponMasterController>(GetController());

	UE_LOG(LogTemp, Display, TEXT("BindInputFunctions: Call"));

	if (!IsValid(WeaponMasterController))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::BindInputFunctions : Controller Casting Failed."));
		return;
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent))
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : EnhancedInputComponent Casting Failed."));
		return;
	}

	// Jumping
	if (WeaponMasterController->JumpAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->JumpAction, ETriggerEvent::Triggered,
		                                   StateComponent.Get(), &UStateComponent::JumpOrDrop);
		EnhancedInputComponent->BindAction(WeaponMasterController->JumpAction, ETriggerEvent::Completed,
		                                   StateComponent.Get(), &UStateComponent::ReleaseJump);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->JumpAction"));
	}

	// Crouching/Dropping
	if (WeaponMasterController->CrouchDropAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->CrouchDropAction, ETriggerEvent::Triggered,
		                                   StateComponent.Get(), &UStateComponent::CrouchDrop);
		EnhancedInputComponent->BindAction(WeaponMasterController->CrouchDropAction, ETriggerEvent::Completed,
		                                   StateComponent.Get(), &UStateComponent::StopCrouchDrop);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->CrouchDropAction"));
	}

	// Moving
	if (WeaponMasterController->MoveAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->MoveAction, ETriggerEvent::Triggered,
		                                   StateComponent.Get(), &UStateComponent::Move);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->MoveAction"));
	}

	// Dashing
	if (WeaponMasterController->DashAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->DashAction, ETriggerEvent::Started,
		                                   StateComponent.Get(), &UStateComponent::Dash);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->DashAction"));
	}

	// WeakAttack
	if (WeaponMasterController->WeakAttackAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->WeakAttackAction, ETriggerEvent::Started,
		                                   StateComponent.Get(), &UStateComponent::WeakAttack);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}

	// StrongAttack
	if (WeaponMasterController->StrongAttackAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->StrongAttackAction, ETriggerEvent::Started,
		                                   StateComponent.Get(), &UStateComponent::StrongAttack);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->StrongAttackAction"));
	}

	// Identity
	if (WeaponMasterController->IdentityAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->IdentityAction, ETriggerEvent::Started,
		                                   StateComponent.Get(), &UStateComponent::Identity);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->IdentityAction"));
	}

	// Defence
	if (WeaponMasterController->DefenceAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->DefenceAction, ETriggerEvent::Started,
		                                   StateComponent.Get(), &UStateComponent::Defence);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->DefenceAction"));
	}

	// PickingItem
	if (WeaponMasterController->PickingItemAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->PickingItemAction, ETriggerEvent::Started,
		                                   StateComponent.Get(), &UStateComponent::PickingItem);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->PickingItemAction"));
	}

	// MenuOnOff
	if (WeaponMasterController->MenuOnOffAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->MenuOnOffAction, ETriggerEvent::Started,
		                                   this, &ABaseBattleCharacter::MenuOnOff);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->MenuOnOffAction"));
	}

}

void ABaseBattleCharacter::SetHP(float NewHP)
{
	float ClampedHP = FMath::Clamp(NewHP, 0.f, MaxHP);
	HP = ClampedHP;
    
	// 명시적 UI 업데이트 코드 추가
	if (auto PC = Cast<APlayerController>(GetController()))
	{
		if (auto SingleHUD = Cast<ASingleGameHUD>(PC->GetHUD()))
		{
			FPlayerStatusInfo StatusInfo;
			StatusInfo.PlayerName = PC->PlayerState ? PC->PlayerState->GetPlayerName() : FString("Player");
			StatusInfo.CurrentHealth = HP;
			StatusInfo.MaxHealth = MaxHP;
			StatusInfo.PlayerThumbnailTexture = GetCharacterThumbnail();
			StatusInfo.CharacterID = 0;
			StatusInfo.TeamID = 0;
            
			SingleHUD->UpdatePlayerStatus(StatusInfo);
		}
	}
    
	// 기존 코드 유지
	if (HasAuthority())
	{
		if (HP <= 0.f) OnDeath();
	}
	else
	{
		ServerSetHP(NewHP);
	}
}

void ABaseBattleCharacter::ServerSetHP_Implementation(float NewHP)
{
	// ㅈㅍㅈㅍ
	UE_LOG(LogTemp, Display, TEXT("ServerSetHP : NewHP : %f"), NewHP);
	float ClampedHP = FMath::Clamp(NewHP, 0.f, MaxHP);

	HP = ClampedHP;

	if (HP <= 0.f)
	{
		UE_LOG(LogTemp, Display, TEXT("Die!"));
		OnDeath();
	}

	// for (auto Debuff : EffectComponent->GetActiveBehaviorEffects())
	// {
	// 	UE_LOG(LogTemp, Display, TEXT("Debuff name: %s"), *StaticEnum<EBehaviorEffect>()->GetNameStringByValue((uint8)Debuff))
	// }
}

void ABaseBattleCharacter::OnDeath()
{
	if (HasAuthority())
	{
		// 사망 이펙트 적용
		EffectComponent->ActivateBehaviorEffect(EBehaviorEffect::Death);

		// Pawn과 충돌 무시
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

		// 플레이어 컨트롤러 가져오기
		APlayerController* PlayerPC = Cast<APlayerController>(GetController());
        
		// 플레이어 사망 통계 업데이트
		if (PlayerPC && PlayerPC->PlayerState)
		{
			AWeaponMasterPlayerState* PS = Cast<AWeaponMasterPlayerState>(PlayerPC->PlayerState);
			if (PS)
			{
				PS->AddDeath();
			}
		}

		// 리플리케이트 안되는 설정 클라에서도 해주기.
		MulticastOnDeath();

		// 2초 후에 게임모드에 사망 알리고 Destroy
		GetWorldTimerManager().SetTimer(
			RespawnDelayTimerHandle,
			FTimerDelegate::CreateLambda([this]()
			{
				// 게임 모드에 사망 알림
				if (auto GM = Cast<IBattleGMInterface>(UGameplayStatics::GetGameMode(this)))
				{
					if (auto CastedGameInstance = Cast<UWeaponMasterGameInstance>(GetGameInstance()))
					{
						TSubclassOf<ACharacter> CharacterClass = CastedGameInstance->CharacterClass;
						FName ItemName = CastedGameInstance->ItemName;

						if (AWeaponMasterController* WMController = Cast<AWeaponMasterController>(GetController()))
						{
							GM->HandlePlayerDeath(CharacterClass, WMController, GetPlayerState()->GetPlayerName());
						}
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("AWeaponMasterController::OnDeath : GameInstance Cast Failed."))
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ABaseBattleCharacter::OnDeath : IBattleGMInterface Cast Failed"));
				}
				Destroy();
			}),
		2.0f,
		false
		);
	}
}

void ABaseBattleCharacter::MulticastOnDeath_Implementation()
{
	// Pawn과 충돌 무시
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

UItemComponent* ABaseBattleCharacter::GetItemComponent_Implementation() const
{
	return ItemComponent;
}

USkillComponent* ABaseBattleCharacter::GetSkillComponent_Implementation() const
{
	return SkillComponent;
}


bool ABaseBattleCharacter::EquipItem_Implementation(FName ItemID)
{
	if (IsValid(ItemComponent))
	{
		return ItemComponent->EquipItem(ItemID);
	}
	return false;
}

void ABaseBattleCharacter::ExecuteSkill_Implementation(int32 SkillIndex)
{
	if (IsValid(SkillComponent))
	{
		SkillComponent->ExecuteSkill(SkillIndex);
	}
}

UStateComponent* ABaseBattleCharacter::GetStateComponent_Implementation() const
{
	return StateComponent;
}

TScriptInterface<UBehaviorState> ABaseBattleCharacter::GetBehaviorState_Implementation() const
{
	return StateComponent->GetBehaviorState();
}

void ABaseBattleCharacter::SetBehaviorState_Implementation(
	const TScriptInterface<UBehaviorState>& NewState)
{
	if (IsValid(StateComponent))
	{
		StateComponent->SetBehaviorState(NewState);
	}
}

void ABaseBattleCharacter::OnItemEquipped_Implementation(UItemDataAsset* EquippedItem)
{
	if (IsValid(SkillComponent))
	{
		SkillComponent->InitializeSkillsFromItem(EquippedItem);
	}
}

void ABaseBattleCharacter::OnItemEquippedForBinding(UItemDataAsset* EquippedItem)
{
	OnItemEquipped_Implementation(EquippedItem);
}

void ABaseBattleCharacter::OnItemUnequipped_Implementation()
{
	if (IsValid(SkillComponent))
	{
		// 스킬 초기화 (null 전달하여 스킬 제거)
		SkillComponent->InitializeSkillsFromItem(nullptr);
	}
}

void ABaseBattleCharacter::OnItemUnequippedForBinding()
{
	OnItemUnequipped_Implementation();
}

void ABaseBattleCharacter::InterruptActiveSkill_Implementation()
{
	// SkillComponent가 없으면 리턴
	if (!SkillComponent)
	{
		return;
	}

	// 현재 활성화된 스킬 배열 가져오기
	TArray<UBaseSkill*> CurrentSkills = SkillComponent->GetSkills();

	// 활성화된 스킬이 있는지 확인하고 중단
	for (UBaseSkill* Skill : CurrentSkills)
	{
		if (Skill && Skill->IsSkillActive())
		{
			// 애니메이션 몽타주 중단
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
				if (CurrentMontage)
				{
					AnimInstance->Montage_Stop(0.25f, CurrentMontage);
				}
			}

			// 스킬 강제 종료
			Skill->EndSkill();

			// 디버그 로그
			// UE_LOG(LogTestCharacter, Display, TEXT("Skill %s was interrupted due to damage"), *Skill->GetSkillName());
			break;
		}
	}
}

void ABaseBattleCharacter::SetInteractableActor_Implementation(AActor* NewInteractableActor)
{
	InteractableActor = NewInteractableActor;
}

AActor* ABaseBattleCharacter::GetInteractableActor_Implementation() const
{
	return InteractableActor;
}

void ABaseBattleCharacter::OnAttacked(const FAttackData& AttackData)
{
	if (HasAuthority())
	{
		// 죽었으면 피격당하지 않도록
		if (EffectComponent->GetActiveBehaviorEffects().Contains(EBehaviorEffect::Death))
		{
			return;
		}
		
		if (GetActorForwardVector().X * AttackData.LaunchVector.X > 0.f)
		{
			UE_LOG(LogTemp, Display, TEXT("OnAttacked : 180 Rotate"));
			SSTCharacterMovementComponent->RequestTurnAround();
		}

		LaunchCharacter(AttackData.LaunchVector, true, true);

		// 데미지 처리 전 기존 HP 저장
		float OldHP = HP;
		SetHP(HP - AttackData.Damage);

		// 이펙트 적용
		for (int32 i = 0; i < AttackData.BehaviorEffects.Num(); i++)
		{
			EffectComponent->ActivateBehaviorEffectWithDuration(AttackData.BehaviorEffects[i],
																AttackData.BehaviorEffectsDurations[i]);
		}
		
		// 공격자의 데미지 통계 업데이트
		auto CastedAttacker = Cast<ACharacter>(AttackData.Attacker);

		// 마지막 공격자 5초동안 기억
		if (IsValid(CastedAttacker))
		{
			LastAttacker = CastedAttacker;
			GetWorldTimerManager().ClearTimer(LastAttackerTimerHandle);;
			GetWorldTimerManager().SetTimer(
				LastAttackerTimerHandle,
				FTimerDelegate::CreateLambda([this]()
				{
					LastAttacker = nullptr;
				}),
				5.0f,
				false
			);
		}
		else if (IsValid(LastAttacker))
		{
			CastedAttacker = LastAttacker;
		}
		else
		{
			// 5초동안 아무한테도 안맞았는데 죽음(킬존)
			return;
		}
		
		APlayerController* AttackerPC = Cast<APlayerController>(CastedAttacker->GetController());
		if (AttackerPC && AttackerPC->PlayerState)
		{
			AWeaponMasterPlayerState* AttackerPS = Cast<AWeaponMasterPlayerState>(AttackerPC->PlayerState);
			if (AttackerPS)
			{
				// 실제 입힌 데미지 계산 (기존 HP - 새 HP, 죽은 경우는 기존 HP만큼)
				float ActualDamage = (HP <= 0.f) ? OldHP : (OldHP - HP);
				AttackerPS->AddDamageDealt(ActualDamage);

				// 죽었으면 킬 카운트 증가
				if (HP <= 0.f)
				{
					AttackerPS->AddKill();
				}
				UE_LOG(LogTemp, Warning, TEXT("[%s]의 현재 전투 통계 - 킬: %d, 총 데미지: %.2f"),
				       *AttackerPS->GetPlayerName(), AttackerPS->GetKillCount(), AttackerPS->GetTotalDamageDealt());
			}
		}

		// 현재 디버프 목록
		for (auto Debuff : EffectComponent->GetActiveBehaviorEffects())
		{
			UE_LOG(LogTemp, Display, TEXT("Current Debuff name: %s"),
				   *StaticEnum<EBehaviorEffect>()->GetNameStringByValue((uint8)Debuff))
		}
		// 현재 HP
		UE_LOG(LogTemp, Display, TEXT("Current HP: %f"), HP);
	}
}

void ABaseBattleCharacter::PlaySkillMontage_Implementation(UAnimMontage* Montage, float PlayRate)
{
	if (HasAuthority()) // 서버에서만 RPC 호출
	{
		Multicast_PlayMontage(Montage, PlayRate);
	}
}

void ABaseBattleCharacter::OnLocalMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 현재 활성화된 스킬 찾기
	if (SkillComponent)
	{
		UBaseSkill* ActiveSkill = SkillComponent->GetActiveSkill();
		if (ActiveSkill)
		{
			// 클라이언트에서도 스킬 종료 처리
			ActiveSkill->EndSkill();
		}
	}
}

UTexture2D* ABaseBattleCharacter::GetCharacterThumbnail() const
{
	if (!CharacterThumbnail.IsNull())
	{
		return CharacterThumbnail.LoadSynchronous();
	}
	return nullptr;
}

void ABaseBattleCharacter::Server_RequestItemPickup_Implementation(AActor* ItemActor)
{
	RequestItemPickup_Implementation(ItemActor);
}

void ABaseBattleCharacter::WeakAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::WeakAttack !"));
	IBattleSystemUser::Execute_ExecuteSkill(this, 0);
}

void ABaseBattleCharacter::StrongAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::StrongAttack !"));
	IBattleSystemUser::Execute_ExecuteSkill(this, 1);
}

void ABaseBattleCharacter::Identity()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::Identity !"));
	IBattleSystemUser::Execute_ExecuteSkill(this, 2);
}

void ABaseBattleCharacter::Defence()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::Defence !"));
}

void ABaseBattleCharacter::PickingItem()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::PickingItem !"));

	if (GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
	{
		AActor* interactableActor = IBattleSystemUser::Execute_GetInteractableActor(this);
		if (interactableActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("상호작용 가능한 아이템 찾음: %s"), *interactableActor->GetName());

			UInteractionComponent* InteractionComp = interactableActor->FindComponentByClass<UInteractionComponent>();
			if (InteractionComp)
			{
				InteractionComp->Interact(this);
			}
		}
	}
}

void ABaseBattleCharacter::OnSkillStarted(UBaseSkill* Skill)
{
	UE_LOG(LogTemp, Display, TEXT("ABaseBattleCharacter::OnSkillStarted : Call."));
	EffectComponent->ActivateBehaviorEffect(EBehaviorEffect::UsingSkill);
}

void ABaseBattleCharacter::OnSkillEnded(UBaseSkill* Skill)
{
	UE_LOG(LogTemp, Display, TEXT("ABaseBattleCharacter::OnSkillEnded : Call."));
	EffectComponent->DeactivateBehaviorEffect(EBehaviorEffect::UsingSkill);
}

void ABaseBattleCharacter::MenuOnOff()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::MenuOnOff !"));
	
	for (auto Debuff : EffectComponent->GetActiveBehaviorEffects())
	{
		UE_LOG(LogTemp, Display, TEXT("Current Debuff name: %s"),
		       *StaticEnum<EBehaviorEffect>()->GetNameStringByValue((uint8)Debuff))
	}

	auto KillCount = Cast<AWeaponMasterPlayerState>(GetPlayerState())->GetKillCount();
	auto DeathCount = Cast<AWeaponMasterPlayerState>(GetPlayerState())->GetDeathCount();

	UE_LOG(LogTemp, Display, TEXT("Kill/Death Count : %d, %d"), KillCount, DeathCount);
}


void ABaseBattleCharacter::Multicast_PlayMontage_Implementation(UAnimMontage* Montage, float PlayRate)
{
	if (!Montage || !IsValid(GetMesh()) || !GetMesh()->GetAnimInstance())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s] 몽타주 %s 재생 시작 (재생속도: %.2f)"),
	       HasAuthority() ? TEXT("서버") : TEXT("클라이언트"),
	       *Montage->GetName(),
	       PlayRate);

	// 모든 클라이언트(및 서버)에서 몽타주 재생
	GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
}

void ABaseBattleCharacter::SetupMontageEndedDelegate_Implementation()
{
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ABaseBattleCharacter::OnLocalMontageEnded);
	}
}

void ABaseBattleCharacter::RequestItemPickup_Implementation(AActor* ItemActor)
{
	UE_LOG(LogTemp, Warning, TEXT("RequestItemPickup: %s가 아이템 %s 획득 요청 (권한: %d)"),
	       *GetName(), ItemActor ? *ItemActor->GetName() : TEXT("Unknown"),
	       (int32)GetLocalRole());

	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("RequestItemPickup: 클라이언트에서 Server_RequestItemPickup 호출"));
		Server_RequestItemPickup(ItemActor);
	}
	else
	{
		// 서버에서 직접 처리
		UE_LOG(LogTemp, Warning, TEXT("RequestItemPickup: 서버에서 직접 ProcessPickup 호출"));
		APickupableItem* PickupItem = Cast<APickupableItem>(ItemActor);
		if (PickupItem)
		{
			PickupItem->ProcessPickup(this);
		}
	}
}
