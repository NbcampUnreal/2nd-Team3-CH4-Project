// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Characters/Components/DamageSystemUser.h"
#include "Characters/Components/IBattleSystemUser.h"

AProjectile::AProjectile()
{
	// 액터 설정
	PrimaryActorTick.bCanEverTick = true;

	// 메시 컴포넌트 설정
	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
	SetRootComponent(ProjectileMeshComponent);
	ProjectileMeshComponent->SetSimulatePhysics(false);
	ProjectileMeshComponent->SetEnableGravity(false);
	ProjectileMeshComponent->SetCollisionProfileName(TEXT("Projectile"));
	ProjectileMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	ProjectileMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	ProjectileMeshComponent->SetGenerateOverlapEvents(true);

	// 프로젝타일 무브먼트 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->Bounciness = 0.6f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// 네트워크 설정
	bReplicates = true;

	// 수명 설정 (5초 후 자동 삭제)
	InitialLifeSpan = 5.0f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 물리 관련 설정 확인
	if (ProjectileMeshComponent)
	{
		ProjectileMeshComponent->SetSimulatePhysics(false);
		ProjectileMeshComponent->SetEnableGravity(false);
	}


	// 프로젝타일 무브먼트 속성 초기화
	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = InitialSpeed;
		ProjectileMovement->MaxSpeed = MaxSpeed;
		ProjectileMovement->ProjectileGravityScale = GravityScale;

		// 회전 관련 설정 적용
		ProjectileMovement->bRotationFollowsVelocity = !bRotationFollowsVelocity;

		// 사용자 정의 회전을 사용할 경우 Tick 활성화
		PrimaryActorTick.bCanEverTick = bRotationFollowsVelocity &&
			(CustomRotationRate.Pitch != 0.0f || CustomRotationRate.Yaw != 0.0f || CustomRotationRate.Roll != 0.0f);
	}

	// 외형 변경이 가능하도록 동적 메시 설정
	if (ProjectileMesh)
	{
		ProjectileMeshComponent->SetStaticMesh(ProjectileMesh);
	}

	// 충돌 이벤트 바인딩 설정
	if (bIsPiercing)
	{
		// 피어싱 발사체는 오버랩 이벤트 사용
		ProjectileMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ProjectileMeshComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
		ProjectileMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlap);
	}
	else
	{
		// 일반 발사체는 히트 이벤트 사용
		ProjectileMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ProjectileMeshComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}

	// 발사 효과음 재생
	if (!ProjectileSound.IsNull())
	{
		USoundBase* Sound = ProjectileSound.LoadSynchronous();
		if (Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
		}
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (bRotationFollowsVelocity)
	{
		// 현재 회전에 사용자 정의 회전 속도를 적용
		FRotator CurrentRotation = GetActorRotation();
		FRotator AddRotation = CustomRotationRate * DeltaTime;

		// 새 회전 설정
		SetActorRotation(CurrentRotation + AddRotation);
	}
}

void AProjectile::FireProjectile(const FVector& Direction)
{
	if (ProjectileMovement)
	{
		// 방향 정규화
		FVector NormalizedDirection = Direction.GetSafeNormal();

		// 디버그 출력 추가
		UE_LOG(LogTemp, Warning, TEXT("Firing projectile with direction: %s, Speed: %f"),
		       *NormalizedDirection.ToString(), InitialSpeed);

		// 발사체 회전 설정
		SetActorRotation(NormalizedDirection.Rotation());

		// 속도 설정
		ProjectileMovement->Velocity = NormalizedDirection * InitialSpeed;

		// 이동 컴포넌트 활성화 확인
		ProjectileMovement->SetActive(true);
		ProjectileMovement->SetUpdatedComponent(ProjectileMeshComponent);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	// 자기 자신이나 발사자는 공격하지 않음
	if (!OtherActor || OtherActor == GetOwner() || OtherActor == GetInstigator())
	{
		return;
	}

	if (ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor))
	{
		// 이미 충돌한 타겟은 무시 (안전 장치)
		if (HitTargets.Contains(OtherCharacter))
		{
			return;
		}

		// 타겟 추가
		AddHitTarget(OtherCharacter);

		// 범위 데미지 로직 제거

		// 직접 타격 대상에 데미지 적용
		ApplyDamage(OtherCharacter, Hit);

		// 이펙트 스폰
		SpawnImpactEffect(Hit);

		// 피어싱이 아니라면 발사체 제거
		if (!bIsPiercing || HitTargets.Num() >= MaxPierceCount)
		{
			Destroy();
		}
	}
}

void AProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 자기 자신이나 발사자는 공격하지 않음
	if (!OtherActor || OtherActor == GetOwner() || OtherActor == GetInstigator())
	{
		return;
	}

	// 이미 충돌한 타겟은 무시
	if (HitTargets.Contains(OtherActor))
	{
		return;
	}

	// 타겟 추가
	AddHitTarget(OtherActor);

	// 데미지 적용
	ApplyDamage(OtherActor, SweepResult);

	// 이펙트 스폰
	SpawnImpactEffect(SweepResult);

	// 최대 관통 횟수에 도달하면 발사체 제거
	if (HitTargets.Num() >= MaxPierceCount)
	{
		Destroy();
	}
}

void AProjectile::AddHitTarget(AActor* Target)
{
	if (Target && !HitTargets.Contains(Target))
	{
		HitTargets.Add(Target);
	}
}

void AProjectile::ApplyDamage(AActor* TargetActor, const FHitResult& Hit)
{
	if (!TargetActor)
	{
		return;
	}

	// 데미지 적용

	// OnAttacked 호출
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	if (TargetCharacter)
	{
		FVector KnockbackDirection = (TargetCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		KnockbackDirection.Z = 0.2f; // 약간 위로 밀기

		if (auto CastedCharacter = Cast<IDamageSystemUser>(TargetActor))
		{
			CastedCharacter->OnAttacked(
				{
					Damage,
					KnockbackDirection * 100,
					{},
					{},
					{},
					{}
				});
		}
	}


	// 넉백 적용 (선택적)
	// if (KnockbackPower > 0.0f)
	// {
	//     ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	//     if (TargetCharacter)
	//     {
	//         FVector KnockbackDirection = (TargetCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	//         KnockbackDirection.Z = 0.2f; // 약간 위로 밀기
	//         
	//         TargetCharacter->LaunchCharacter(KnockbackDirection * KnockbackPower, true, true);
	//     }
	// }
}

void AProjectile::SpawnImpactEffect(const FHitResult& Hit)
{
	// 충돌 이펙트 재생
	if (!ImpactEffect.IsNull())
	{
		UNiagaraSystem* Effect = ImpactEffect.LoadSynchronous();
		if (Effect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				Effect,
				Hit.Location,
				Hit.Normal.Rotation()
			);
		}
	}

	// 충돌 사운드 재생
	if (!ImpactSound.IsNull())
	{
		USoundBase* Sound = ImpactSound.LoadSynchronous();
		if (Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Sound, Hit.Location);
		}
	}
}
