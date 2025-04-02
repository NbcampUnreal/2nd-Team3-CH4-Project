// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponItemBase.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"


// Sets default values
AWeaponItemBase::AWeaponItemBase()
{
	// 공격 콜리전 설정
	AttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollision"));
	AttackCollision->SetupAttachment(ItemMesh);
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponItemBase::OnAttackOverlap);

	// 기본 소켓 이름
	AttachSocketName = "WeaponSocket";
	
	// 현재 스킬 인덱스 초기화
	CurrentSkillIndex = -1;
}

// Called when the game starts or when spawned
void AWeaponItemBase::BeginPlay()
{
	Super::BeginPlay();

	SkillCooldownRemaining.Init(0.0f, Skills.Num());
}

void AWeaponItemBase::OnEquip_Implementation(ACharacter* Character)
{
	Super::OnEquip_Implementation(Character);

	UE_LOG(LogTemp, Warning, TEXT(" [AWeaponItemBase::OnEquip_Implementation] 오버랩됨"));
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT(" [AWeaponItemBase::OnEquip_Implementation] 캐릭터임"));
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketName);
		
		// 콜리전 비활성화
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		// 캐릭터에게 무기 참조 설정
		//Character->SetEquippedWeapon(this);
	}
}

bool AWeaponItemBase::UseSkill(int32 SkillIndex, ACharacter* Character)
{
	// 유효한 스킬 인덱스 확인
	if (!Skills.IsValidIndex(SkillIndex) || !Character)
	{
		return false;
	}
	
	// 쿨타임 체크
	if (SkillCooldownRemaining[SkillIndex] > 0.0f)
	{
		// 여기서 이제 쿨타임이 안됐다고 플레이어에게 알림을 보내는 기능을 구현해도 괜찮을 듯함
		return false;
	}
	
	// 현재 스킬 인덱스 설정
	CurrentSkillIndex = SkillIndex;

	checkf(Skills[SkillIndex].SkillMontage, TEXT("Skill Montage is not set."));
	
	// 애니메이션 몽타주 재생
	if (Skills[SkillIndex].SkillMontage)
	{
		float MontageLength = Character->PlayAnimMontage(Skills[SkillIndex].SkillMontage);
		
		// 쿨타임 시작
		SkillCooldownRemaining[SkillIndex] = Skills[SkillIndex].Cooldown;
		
		// 쿨타임 타이머 설정
		FTimerHandle CooldownTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			CooldownTimerHandle,
			[this, SkillIndex]()
			{
				SkillCooldownRemaining[SkillIndex] = 0.0f;
			},
			Skills[SkillIndex].Cooldown,
			false
		);
		
		// UI 업데이트 알림(보류)
		//Character->NotifySkillCooldownStarted(SkillIndex, Skills[SkillIndex].Cooldown);
		
		return true;
	}
	
	return false;
}

void AWeaponItemBase::EnableAttackCollision()
{
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeaponItemBase::DisableAttackCollision()
{
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponItemBase::OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* MyOwner = GetOwner();
	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		// 현재 스킬 데이터 가져오기
		if (Skills.IsValidIndex(CurrentSkillIndex))
		{
			// 대미지 적용
			FSkillData& CurrentSkill = Skills[CurrentSkillIndex];
			
			FDamageEvent DamageEvent;
			OtherActor->TakeDamage(CurrentSkill.Damage, DamageEvent, MyOwner ? MyOwner->GetInstigatorController() : nullptr, this);
		}
	}
}

