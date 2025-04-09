#include "WeaponMaster/AI/BossCharacter.h"
#include "WeaponMaster/AI/WeaponMasterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WeaponMaster/Characters/WeaponMAsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"


ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MaxHP = 1000;

	BossStateComponent = CreateDefaultSubobject<UBossStateComponent>(TEXT("BossStateComponent"));

	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 1.f, 0.f));
	SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;

	if (BossStateComponent)
	{
		BossStateComponent->OnPhaseChanged.AddDynamic(this, &ABossCharacter::OnPhaseChanged);
	}

	if (SkillComponent && BossSkillAsset)
	{
		SkillComponent->InitializeSkills(BossSkillAsset);
	}

}


void ABossCharacter::OnPhaseChanged(EBossPhase NewPhase)
{
	UE_LOG(LogTemp, Warning, TEXT("보스 페이즈 변경됨: %d"), (uint8)NewPhase);

	if (AWeaponMasterAIController* AICon = Cast<AWeaponMasterAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			BB->SetValueAsEnum(TEXT("EBossPhase"), static_cast<uint8>(NewPhase));
		}
	}
}

void ABossCharacter::LookAtTarget(const AActor* TargetActor)
{
	if (!TargetActor) return;

	FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.f; // 수평 회전만

	if (!ToTarget.IsNearlyZero())
	{
		FRotator LookAtRotation = ToTarget.Rotation();
		SetActorRotation(LookAtRotation);
	}
}


void ABossCharacter::Multicast_PlayMontage_Implementation(UAnimMontage* Montage)
{
	if (Montage)
	{
		PlayAnimMontage(Montage);
	}
}

void ABossCharacter::ApplyBasicCombo()
{
	if (HasAuthority())
	{
		if (SkillComponent)
		{
			SkillComponent->ExecuteSkill(0); // 0번째 스킬 실행
			UE_LOG(LogTemp, Warning, TEXT("BossSkill 0"));
		}
	}
}

void ABossCharacter::ApplyBackStep()
{
	if (!HasAuthority()) return;

	FVector BackDir = -GetActorForwardVector();

	FVector LaunchVelocity = BackDir * 1200.0f + FVector(0, 0, 200.0f);

	LaunchCharacter(LaunchVelocity, true, true);
}

void ABossCharacter::ApplyForwardCharge()
{
	if (!HasAuthority()) return;

	 Multicast_PlayMontage(ChargeMontage);

	FTimerHandle LaunchTimerHandle;
	GetWorldTimerManager().SetTimer(LaunchTimerHandle, this, &ABossCharacter::PerformForwardCharge, 0.5f, false);

}

void ABossCharacter::PerformForwardCharge()
{
	FVector ChargeDir = GetActorForwardVector();
	FVector LaunchVelocity = ChargeDir * 2000.f + FVector(0, 0, 150.f);

	LaunchCharacter(LaunchVelocity, true, true);
}

void ABossCharacter::ApplyAreaSkill()
{
	// 1. 방향 돌리기
	SetActorRotation(FRotator(0, 90.f, 0));

	// 2. 기모으기 애니메이션
	//Multicast_PlayMontage(ChargeMontage);

	// 3. 타이머로 실제 시전
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this, &ABossCharacter::ExecuteAreaSkill, 2.0f, false);
}

void ABossCharacter::ExecuteAreaSkill()
{
	if (HasAuthority())
	{
		//StartBasicCombo();
		if (SkillComponent)
		{
			SkillComponent->ExecuteSkill(1);
			UE_LOG(LogTemp, Warning, TEXT("BossSkill 1"));
		}
	}
}

void ABossCharacter::ApplyPowerAttack()
{
	if (HasAuthority())
	{
		if (SkillComponent)
		{
			SkillComponent->ExecuteSkill(2); // 0번째 스킬 실행
			UE_LOG(LogTemp, Warning, TEXT("BossSkill 2"));
		}
	}
}

void ABossCharacter::OnAttacked(int Damage)
{
	CurrentHP -= Damage;
	if (CurrentHP <= 0)
	{
		CurrentHP = 0;
		//Die();
	}
}

void ABossCharacter::Die()
{
	//SetIsDeath(true);

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	// AI 멈춤
	AAIController* AICon = Cast<AAIController>(GetController());
	if (AICon)
	{
		AICon->StopMovement();
	}
}

// !~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!
// !~!~!~!~ Battle System User Interface ~!~!~!~!
// !~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!

UItemComponent* ABossCharacter::GetItemComponent_Implementation() const
{
	return nullptr;
}

USkillComponent* ABossCharacter::GetSkillComponent_Implementation() const
{
	return SkillComponent;
}


bool ABossCharacter::EquipItem_Implementation(FName ItemID)
{
	return true;
}

void ABossCharacter::ExecuteSkill_Implementation(int32 SkillIndex)
{
}

UCharacterBehaviorState* ABossCharacter::GetBehaviorState_Implementation() const
{
	return nullptr;
}

void ABossCharacter::OnItemEquipped_Implementation(UItemDataAsset* EquippedItem)
{
}

void ABossCharacter::OnItemUnequipped_Implementation()
{
}

void ABossCharacter::InterruptActiveSkill_Implementation()
{
}

void ABossCharacter::SetInteractableActor_Implementation(AActor* NewInteractableActor)
{
}

AActor* ABossCharacter::GetInteractableActor_Implementation() const
{
	return nullptr;
}


void ABossCharacter::OnAttacked(const FAttackData& AttackData)
{
}