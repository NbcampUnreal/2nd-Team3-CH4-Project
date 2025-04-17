#include "AI/AICharacter/BossCharacter.h"
#include "WeaponMaster/AI/AIController/WeaponMasterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WeaponMaster/Characters/WeaponMAsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerState/WeaponMasterPlayerState.h"
#include "GameModes/TeamGameMode.h"

ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MaxHP = 2000;

	BossStateComponent = CreateDefaultSubobject<UBossStateComponent>(TEXT("BossStateComponent"));
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 1.f, 0.f));
	SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));

	UE_LOG(LogTemp, Warning, TEXT("[Boss] OrientToMove: %d | UseYaw: %d | YawRate: %f"),
		GetCharacterMovement()->bOrientRotationToMovement,
		bUseControllerRotationYaw,
		GetCharacterMovement()->RotationRate.Yaw);

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

	if (!HasAuthority())
	{
		SetupMontageEndedDelegate_Implementation(); 
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


void ABossCharacter::Multicast_PlayMontage_Implementation(UAnimMontage* Montage, float PlayRate)
{
	if (!Montage || !IsValid(GetMesh()) || !GetMesh()->GetAnimInstance())
	{
		return;
	}
    
	UE_LOG(LogTemp, Warning, TEXT("[보스:%s] 몽타주 %s 재생 시작 (재생속도: %.2f)"), 
		HasAuthority() ? TEXT("서버") : TEXT("클라이언트"),
		*Montage->GetName(), 
		PlayRate);
        
	// 모든 클라이언트(및 서버)에서 몽타주 재생
	GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
}

void ABossCharacter::ApplyBasicCombo()
{
	if (HasAuthority())
	{
		if (SkillComponent)
		{
			SkillComponent->ExecuteSkill(0);
			SetIsAttackingForDuration(1.4f);
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
	if (HasAuthority())
	{
		if (SkillComponent)
		{
			SkillComponent->ExecuteSkill(2);
			SetIsAttackingForDuration(1.6f);
		}
	}

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
	SetActorRotation(FRotator(0, 90.f, 0));
	Multicast_PlayMontage(AreaChargeMontage,1);
	SetIsAttackingForDuration(5.0f);
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this, &ABossCharacter::ExecuteAreaSkill, 2.0f, false);
}

void ABossCharacter::ExecuteAreaSkill()
{
	if (HasAuthority())
	{
		if (SkillComponent)
		{
			SkillComponent->ExecuteSkill(1);
		}
	}
}

void ABossCharacter::ApplyPowerAttack()
{
	if (HasAuthority())
	{
		if (SkillComponent)
		{
			SkillComponent->ExecuteSkill(3);
			SetIsAttackingForDuration(2.6f);
		}
	}
}

void ABossCharacter::Die()
{
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	// AI 중지
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();

		// 블랙보드에 사망 상태 기록
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			BB->SetValueAsBool(TEXT("bIsDead"), true);
		}
	}

	if (GetWorld())
	{
		AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
		if (ATeamGameMode* TeamGM = Cast<ATeamGameMode>(GM))
		{
			TeamGM->OnBossDefeated();
		}
	}

	SetActorEnableCollision(false); // 피격 안되게
}

void ABossCharacter::SetIsAttackingForDuration(float Duration)
{
	bIsAttacking = true;

	// 기존 타이머 제거 후 새로 시작
	GetWorldTimerManager().ClearTimer(AttackStateTimerHandle);
	GetWorldTimerManager().SetTimer(
		AttackStateTimerHandle,
		this,
		&ABossCharacter::ClearIsAttacking,
		Duration,
		false
	);
}

void ABossCharacter::ClearIsAttacking()
{
	bIsAttacking = false;
}

bool ABossCharacter::IsAttacking()
{
	return bIsAttacking;  // 공격 중일 때 true
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

TScriptInterface<UBehaviorState> ABossCharacter::GetBehaviorState_Implementation() const
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
	auto CastedAttacker = Cast<ACharacter>(AttackData.Attacker);

	APlayerController* AttackerPC = Cast<APlayerController>(CastedAttacker->GetController());
	if (AttackerPC && AttackerPC->PlayerState)
	{
		AWeaponMasterPlayerState* AttackerPS = Cast<AWeaponMasterPlayerState>(AttackerPC->PlayerState);
		if (AttackerPS)
		{
			// 실제 입힌 데미지 계산 (기존 HP - 새 HP, 죽은 경우는 기존 HP만큼)
			float ActualDamage = AttackData.Damage;
			AttackerPS->AddDamageDealt(ActualDamage);
		}
	}

	LaunchCharacter(AttackData.LaunchVector/3, true, true);
	if(CurrentHP - AttackData.Damage <= 0)
	{
		Die();
		UE_LOG(LogTemp, Warning, TEXT("Die CurrentHP : %d"), CurrentHP);
	}
	else
	{
		CurrentHP -= AttackData.Damage;
		UE_LOG(LogTemp, Warning, TEXT("AYA Boss CurrentHP : %d"), CurrentHP);
	}
}

void ABossCharacter::PlaySkillMontage_Implementation(UAnimMontage* Montage, float PlayRate)
{
	if (HasAuthority()) // 서버에서만 RPC 호출
	{
		Multicast_PlayMontage(Montage, PlayRate);
	}
}

void ABossCharacter::SetupMontageEndedDelegate_Implementation()
{
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ABossCharacter::OnLocalMontageEnded);
	}
}

void ABossCharacter::OnLocalMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
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

