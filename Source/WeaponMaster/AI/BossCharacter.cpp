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

void ABossCharacter::PerformBasicAttack()
{
	if (BasicAttackMontage)
	{
		PlayAnimMontage(BasicAttackMontage);
		//타격 타이밍?
	}
	ApplyBasicCombo();
}

void ABossCharacter::ApplyBasicCombo()
{
	PerformComboAttack1();

	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		FTimerHandle Handle2;
		GetWorldTimerManager().SetTimer(Handle2, this, &ABossCharacter::PerformComboAttack2, 0.7f, false);
	});

	FTimerHandle Handle3;
	GetWorldTimerManager().SetTimer(Handle3, this, &ABossCharacter::PerformComboAttack3, 1.4f, false);
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

void ABossCharacter::PerformComboAttack1()
{
	FVector Center, Extent;
	CalculateAttackBox(1, Center, Extent);
	DamageActorsInBox(Center, Extent);
}

void ABossCharacter::PerformComboAttack2()
{
	FVector Center, Extent;
	CalculateAttackBox(2, Center, Extent);
	DamageActorsInBox(Center, Extent);
}

void ABossCharacter::PerformComboAttack3()
{
	FVector Center, Extent;
	CalculateAttackBox(3, Center, Extent);  // 꼭 필요!
	DamageActorsInBox(Center, Extent);
}