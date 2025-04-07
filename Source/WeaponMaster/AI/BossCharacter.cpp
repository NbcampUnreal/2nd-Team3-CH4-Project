#include "WeaponMaster/AI/BossCharacter.h"
#include "WeaponMaster/AI/WeaponMasterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WeaponMaster/Characters/WeaponMAsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MaxHP = 1000.0f;

	BossStateComponent = CreateDefaultSubobject<UBossStateComponent>(TEXT("BossStateComponent"));
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;

	if (BossStateComponent)
	{
		BossStateComponent->OnPhaseChanged.AddDynamic(this, &ABossCharacter::OnPhaseChanged);
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

void ABossCharacter::CalculateAttackBox(int32 ComboStep, FVector& OutCenter, FVector& OutExtent)
{
	FVector Forward = GetActorForwardVector();
	FVector MyLocation = GetActorLocation();

	float ForwardDistance = 200.f + (ComboStep - 1) * 100.f;
	OutCenter = MyLocation + Forward * ForwardDistance + FVector(0, 0, 50);
	OutExtent = FVector(100.f, 200.f, 100.f);

	DrawDebugBox(
		GetWorld(),                    // 월드
		OutCenter,                 // 박스 중심
		OutExtent,                    // 반지름 (절반 사이즈)
		FQuat::Identity,             // 회전 (일단 없음)
		FColor::Red,                 // 색깔
		false,                       // 영구 표시? false면 몇 초만 표시됨
		0.7f,                        // 지속 시간 (초 단위)
		0,                           // Depth Priority
		2.0f                         // 선 두께
	);
}


void ABossCharacter::DamageActorsInBox(const FVector& Center, const FVector& Extent)
{
	DrawDebugBox(GetWorld(), Center, Extent, FQuat::Identity, FColor::Red, false, 1.0f, 0, 2.0f);

	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponMasterCharacter::StaticClass(), AllEnemies);

	for (AActor* Enemy : AllEnemies)
	{
		if (!Enemy || Enemy == this) continue;

		if (UKismetMathLibrary::IsPointInBox(Enemy->GetActorLocation(), Center, Extent))
		{
			// 나중에 HitReactComponent에 연결
			UE_LOG(LogTemp, Warning, TEXT("콤보 공격 타격: %s"), *Enemy->GetName());
		}
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
		StartBasicCombo();
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

	FVector Center, Extent;
	CalculateAttackBox(1, Center, Extent);
	DamageActorsInBox(Center, Extent);
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
	FVector LaunchVelocity = ChargeDir * 4000.f + FVector(0, 0, 100.f);

	LaunchCharacter(LaunchVelocity, true, true);

	// 충돌 타이밍을 고려해 약간 딜레이 후 판정 실행
	//FTimerHandle HitCheckTimer;
	//GetWorldTimerManager().SetTimer(HitCheckTimer, this, &ABossCharacter::ChargeHitCheck, 0.3f, false);
}

void ABossCharacter::ApplyAreaSkill()
{
	// 1. 방향 돌리기
	SetActorRotation(GetActorRotation() + FRotator(0, 90.f, 0));

	// 2. 기모으기 애니메이션
	//Multicast_PlayMontage(Phase2ChargeMontage);

	// 3. 타이머로 실제 시전
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this, &ABossCharacter::ExecuteAreaSkill, 2.0f, false);
}

void ABossCharacter::ExecuteAreaSkill()
{
	const float Radius = 600.f;
	const FVector MyLocation = GetActorLocation();

	DrawDebugSphere(
		GetWorld(),
		MyLocation,
		Radius,
		24,
		FColor::Purple,
		false,
		2.0f,
		0,
		3.0f
	);

	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponMasterCharacter::StaticClass(), AllEnemies);

	for (AActor* Enemy : AllEnemies)
	{
		if (!Enemy || Enemy == this) continue;

		const float Dist = FVector::Dist(Enemy->GetActorLocation(), MyLocation);

		if (Dist <= Radius)
		{
			// 나중에 OnTakeBossDamage 등으로 연결해도 좋음
			UGameplayStatics::ApplyDamage(Enemy, 100.f, GetController(), this, nullptr);
			UE_LOG(LogTemp, Warning, TEXT("범위기에 맞은 액터: %s"), *Enemy->GetName());
		}
	}
}
