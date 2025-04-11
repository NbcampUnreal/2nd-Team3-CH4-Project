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
#include "Items/InteractionComponent/InteractionComponent.h"
#include "WeaponMaster/PlayerControllers/WeaponMasterController.h"
#include "Data/StatusTypes.h"

// Sets default values
ABaseBattleCharacter::ABaseBattleCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USSTCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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

	// Replicate
	bReplicates = true;
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
	// UI 연결 로직
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

	if (StateComponent->bIsComponentReady)
	{
		BindInputFunctions();
	}
	else
	{
		StateComponent->OnStateComponentReady.BindUObject(this, &ABaseBattleCharacter::BindInputFunctions);
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

	// Chat
	if (WeaponMasterController->ChatAction)
	{
		EnhancedInputComponent->BindAction(WeaponMasterController->ChatAction, ETriggerEvent::Started,
										   this, &ABaseBattleCharacter::Chat);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			   TEXT("ABaseBattleCharacter::BindInputFunctions : No WeaponMasterController->ChatAction"));
	}
}

void ABaseBattleCharacter::SetHP(float NewHP)
{
	UE_LOG(LogTemp, Display, TEXT("SetHP : NewHP : %f"), NewHP);
	float ClampedHP = FMath::Clamp(NewHP, 0.f, MaxHP);

	if (HasAuthority())
	{
		HP = ClampedHP;

		if (HP <= 0.f)
		{
			UE_LOG(LogTemp, Display, TEXT("Die!"));
			OnDeath();
		}
	}
	else
	{
		// HP Client Prediction
		HP = ClampedHP;
		
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

void ABaseBattleCharacter::OnDeath() const
{
	if (HasAuthority())
	{
		EffectComponent->ActivateBehaviorEffect(EBehaviorEffect::Death);
	}
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
		// ㅈㅍㅈㅍ
		LaunchCharacter(AttackData.LaunchVector, true, true);
		UE_LOG(LogTemp, Display, TEXT("OnAttacked : %f"), AttackData.LaunchVector.Z);
	
		SetHP(HP - AttackData.Damage);

		// 이펙트 적용
		for (int32 i = 0 ; i < AttackData.BehaviorEffects.Num() ; i++)
		{
			EffectComponent->ActivateBehaviorEffectWithDuration(AttackData.BehaviorEffects[i], AttackData.BehaviorEffectsDurations[i]);
		}
	}
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
	// 상호작용 가능한 아이템이 있는지 확인

	if (GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
	{
		AActor* interactableActor = IBattleSystemUser::Execute_GetInteractableActor(this);
		if (interactableActor)
		{
			UInteractionComponent* InteractionComp = interactableActor->FindComponentByClass<UInteractionComponent>();
			if (InteractionComp)
			{
				InteractionComp->Interact( this);
			}
		}
	}
}

void ABaseBattleCharacter::MenuOnOff()
{
	for (auto Debuff : EffectComponent->GetActiveBehaviorEffects())
	{
		UE_LOG(LogTemp, Display, TEXT("Current Debuff name: %s"), *StaticEnum<EBehaviorEffect>()->GetNameStringByValue((uint8)Debuff))
	}
	
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::MenuOnOff !"));
}

void ABaseBattleCharacter::Chat()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBattleCharacter::Chat"));
}
