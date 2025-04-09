// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBattleCharacter.h"
#include "SSTCharacterMovementComponent.h"
#include "FollowCameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "InputActionValue.h"
#include "Characters/Components/StateComponent/StateComponent.h"
#include "Characters/Components/ItemComponent/ItemComponent.h"
#include "Characters/Components/SkillComponent/SkillComponent.h"
#include "Characters/Components/EffectComponent/EffectComponent.h"
#include "Skills/BaseSkill.h"
#include "Characters/Components/StateComponent/States/CharacterBehaviorState.h"
#include "Items/InteractionComponent/InteractionComponent.h"
#include "WeaponMaster/PlayerControllers/WeaponMasterController.h"
#include "Data/StatusTypes.h"
#include "GameFramework/PlayerState.h"

// Sets default values
ABaseBattleCharacter::ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USSTCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// StateComponent
	StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("StateComponent"));
	EffectComponent = CreateDefaultSubobject<UEffectComponent>(TEXT("EffectComponent"));
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
	InteractableActor = nullptr;

	// Constants
	MaxHP = 100.0f;
	HP = MaxHP;

	// CharacterMovement Constants Setting
	GetCharacterMovement()->AirControl = 1.0f;
	GetCharacterMovement()->AirControlBoostMultiplier = 0.0f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 0.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
}

// Called when the game starts or when spawned
void ABaseBattleCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(ItemComponent) && IsValid(SkillComponent))
	{
		// Item Equipment Event Binding
		ItemComponent->OnItemEquipped.AddDynamic(this, &IBattleSystemUser::OnItemEquipped);
		ItemComponent->OnItemUnequipped.AddDynamic(this, &IBattleSystemUser::OnItemUnequipped);
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

	StateComponent->OnStateComponentReady.BindUObject(this, &ABaseBattleCharacter::BindInputFunctions);
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

	UCharacterBehaviorState* CharacterBehaviorState = StateComponent->GetBehaviorState();

	// Jumping
	if (WeaponMasterController->JumpAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->JumpAction, ETriggerEvent::Triggered,
		                                   CharacterBehaviorState, &UCharacterBehaviorState::JumpOrDrop);
		EnhancedInputComponent->BindAction(WeaponMasterController->JumpAction, ETriggerEvent::Completed,
		                                   CharacterBehaviorState, &UCharacterBehaviorState::ReleaseJump);
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
		                                   CharacterBehaviorState, &UCharacterBehaviorState::CrouchDrop);
		EnhancedInputComponent->BindAction(WeaponMasterController->CrouchDropAction, ETriggerEvent::Completed,
		                                   CharacterBehaviorState, &UCharacterBehaviorState::StopCrouchDrop);
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
		                                   CharacterBehaviorState, &UCharacterBehaviorState::Move);
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
		                                   CharacterBehaviorState, &UCharacterBehaviorState::Dash);
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
		                                   CharacterBehaviorState, &UCharacterBehaviorState::WeakAttack);
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
		                                   CharacterBehaviorState, &UCharacterBehaviorState::StrongAttack);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}

	// Identity
	if (WeaponMasterController->IdentityAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->IdentityAction, ETriggerEvent::Started,
		                                   CharacterBehaviorState, &UCharacterBehaviorState::Identity);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}

	// Defence
	if (WeaponMasterController->DefenceAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->DefenceAction, ETriggerEvent::Started,
		                                   CharacterBehaviorState, &UCharacterBehaviorState::Defence);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}

	// PickingItem
	if (WeaponMasterController->PickingItemAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->PickingItemAction, ETriggerEvent::Started,
		                                   CharacterBehaviorState, &UCharacterBehaviorState::PickingItem);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}

	// MenuOnOff
	if (WeaponMasterController->MenuOnOffAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->MenuOnOffAction, ETriggerEvent::Started,
		                                   CharacterBehaviorState, &UCharacterBehaviorState::MenuOnOff);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->WeakAttackAction"));
	}
}

void ABaseBattleCharacter::SetHP(float NewHP)
{
	if (NewHP > MaxHP)
	{
		HP = 100;
		UE_LOG(LogTemp, Display, TEXT("Max HP"));
	}
	if (NewHP > 0)
	{
		HP = NewHP;
		UE_LOG(LogTemp, Display, TEXT("GetDamaged. HP: %f"), HP);
	}
	else
	{
		HP = 0;
		// 죽는 로직
		UE_LOG(LogTemp, Display, TEXT("Die!"));
	}
}


UItemComponent* ABaseBattleCharacter::GetItemComponent() const
{
	return ItemComponent;
}

USkillComponent* ABaseBattleCharacter::GetSkillComponent() const
{
	return SkillComponent;
}


bool ABaseBattleCharacter::EquipItem(FName ItemID)
{
	if (IsValid(ItemComponent))
	{
		return ItemComponent->EquipItem(ItemID);
	}
	return false;
}

void ABaseBattleCharacter::ExecuteSkill(int32 SkillIndex)
{
	if (IsValid(SkillComponent))
	{
		SkillComponent->ExecuteSkill(SkillIndex);
	}
}

UCharacterBehaviorState* ABaseBattleCharacter::GetBehaviorState() const
{
	return StateComponent->GetBehaviorState();
}

void ABaseBattleCharacter::OnItemEquipped(UItemDataAsset* EquippedItem)
{
	if (IsValid(SkillComponent))
	{
		SkillComponent->InitializeSkillsFromItem(EquippedItem);
	}
}

void ABaseBattleCharacter::OnItemUnequipped()
{
	if (IsValid(SkillComponent))
	{
		// 스킬 초기화 (null 전달하여 스킬 제거)
		SkillComponent->InitializeSkillsFromItem(nullptr);
	}
}

void ABaseBattleCharacter::InterruptActiveSkill()
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

void ABaseBattleCharacter::SetInteractableActor(AActor* NewInteractableActor)
{
	InteractableActor = NewInteractableActor;
}

AActor* ABaseBattleCharacter::GetInteractableActor() const
{
	return InteractableActor;
}

void ABaseBattleCharacter::OnAttacked(const FAttackData& AttackData)
{
	auto a = GetPlayerState()->GetUniqueId();
	auto b = GetPlayerState()->GetPlayerId();
	
	LaunchCharacter(AttackData.LaunchVector, true, true);
	
	SetHP(HP - AttackData.Damage);

	// 이펙트 적용
	for (int32 i = 0 ; i < AttackData.BehaviorEffectNumbers ; i++)
	{
		EffectComponent->ActivateBehaviorEffect(AttackData.BehaviorEffects[i], AttackData.BehaviorEffectsDurations[i]);
	}
}

void ABaseBattleCharacter::WeakAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::WeakAttack !"));
	ExecuteSkill(0);
}

void ABaseBattleCharacter::StrongAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::StrongAttack !"));
	ExecuteSkill(1);
}

void ABaseBattleCharacter::Identity()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::Identity !"));
	ExecuteSkill(2);
}

void ABaseBattleCharacter::Defence()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::Defence !"));
}

void ABaseBattleCharacter::PickingItem()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::PickingItem !"));
	// 상호작용 가능한 아이템이 있는지 확인
	if (GetInteractableActor())
	{
		// 상호작용 컴포넌트 찾기
		UInteractionComponent* InteractionComp = GetInteractableActor()->FindComponentByClass<UInteractionComponent>();
		if (InteractionComp)
		{
			InteractionComp->Interact( this);
		}
	}
}

void ABaseBattleCharacter::MenuOnOff()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::MenuOnOff !"));
}
