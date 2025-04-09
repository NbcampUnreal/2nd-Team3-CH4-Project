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
	MaxHP = 1000.0f;

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
		//StartBasicCombo();
		if (SkillComponent)
		{
			SkillComponent->ExecuteSkill(0); // 0번째 스킬 실행
			UE_LOG(LogTemp, Warning, TEXT("BossItem 0"));
		}
	}
	else
	{
		Server_ApplyBasicCombo();
	}
}


void ABossCharacter::Server_ApplyBasicCombo_Implementation()
{
	StartBasicCombo();
}


void ABossCharacter::StartBasicCombo()
{
	PerformComboAttack();
}

void ABossCharacter::PerformComboAttack()
{
	if (ComboMontage1)
	{
		Multicast_PlayMontage(ComboMontage1);
	}
}

void ABossCharacter::ApplyBackStep()
{
	if (!HasAuthority()) return;

	//Multicast_PlayMontage(BackStepMontage); // 연출용 애니메이션

	FVector BackDir = -GetActorForwardVector();

	FVector LaunchVelocity = BackDir * DashPower + FVector(0, 0, JumpBoost);

	LaunchCharacter(LaunchVelocity, true, true);
}

void ABossCharacter::ExecuteForwardCharge()
{
	if (!HasAuthority()) return;

	// 돌진 애니메이션 재생 (멀티캐스트)
	// Multicast_PlayMontage(ChargeMontage);
	// 돌진 방향 = 현재 보스 정면

	FVector ChargeDir = GetActorForwardVector();
	FVector LaunchVelocity = ChargeDir * 2000.f + FVector(0, 0, 150.f);

	LaunchCharacter(LaunchVelocity, true, true);

	// 충돌 타이밍을 고려해 약간 딜레이 후 판정 실행
	//FTimerHandle HitCheckTimer;
	//GetWorldTimerManager().SetTimer(HitCheckTimer, this, &ABossCharacter::ChargeHitCheck, 0.3f, false);

}

void ABossCharacter::ApplyAreaSkill()
{
	// 1. 방향 돌리기
	SetActorRotation(FRotator(0, 90.f, 0));

	// 2. 기모으기 애니메이션
	//Multicast_PlayMontage(Phase2ChargeMontage);

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
