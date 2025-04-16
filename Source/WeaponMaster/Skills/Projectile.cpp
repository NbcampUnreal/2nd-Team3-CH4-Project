// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Characters/Components/DamageSystemUser.h"

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
	ProjectileMeshComponent->SetGenerateOverlapEvents(true);

	// 프로젝타일 무브먼트 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->Bounciness = 0.6f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	
	// 나이아가라 트레일 이펙트 컴포넌트 추가
	TrailEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffectComponent"));
	TrailEffectComponent->SetupAttachment(ProjectileMeshComponent);
	TrailEffectComponent->bAutoActivate = true;

	// 네트워크 설정
	bReplicates = true;
	SetReplicateMovement(true);
	bNetUseOwnerRelevancy = true;
	
	// 기본 수명 설정 (5초 후 자동 삭제)
	InitialLifeSpan = 5.0f;
	
	// 초기 태그 설정
	Tags.Add(FName("Projectile"));
	
	// 기타 멤버 초기화
	LastImpactTime = 0.0f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 발사체 설정 초기화
	InitializeCollision();
	InitializeMovement();
	InitializeEffects();
	// 사용자 설정에 따라 메시 가시성 설정
	ProjectileMeshComponent->SetVisibility(bMeshVisible);
	// 외형 변경이 가능하도록 동적 메시 설정
	if (ProjectileMesh)
	{
		ProjectileMeshComponent->SetStaticMesh(ProjectileMesh);
	}
	
	// 스폰 시 자동으로 인스티게이터 방향으로 발사
	LaunchFromInstigator();
}

void AProjectile::InitializeCollision()
{
	if (!ProjectileMeshComponent)
	{
		return;
	}

	// 물리 관련 설정 확인
	ProjectileMeshComponent->SetSimulatePhysics(false);
	ProjectileMeshComponent->SetEnableGravity(false);

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
		ProjectileMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		ProjectileMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		ProjectileMeshComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}

void AProjectile::InitializeMovement()
{
	if (!ProjectileMovement)
	{
		return;
	}

	// 프로젝타일 무브먼트 속성 초기화
	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->ProjectileGravityScale = GravityScale;

	// 회전 관련 설정 적용
	ProjectileMovement->bRotationFollowsVelocity = bRotationFollowsVelocity;

	// 사용자 정의 회전을 사용할 경우 Tick 활성화
	PrimaryActorTick.bCanEverTick = !bRotationFollowsVelocity &&
		(CustomRotationRate.Pitch != 0.0f || CustomRotationRate.Yaw != 0.0f || CustomRotationRate.Roll != 0.0f);
}

void AProjectile::InitializeEffects()
{
	// 발사 효과음 재생
	if (!ProjectileSound.IsNull())
	{
		USoundBase* Sound = ProjectileSound.LoadSynchronous();
		if (Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
		}
	}
	
	// 트레일 이펙트 설정
	if (!TrailEffect.IsNull() && TrailEffectComponent)
	{
		UNiagaraSystem* TrailSystem = TrailEffect.LoadSynchronous();
		if (TrailSystem)
		{
			// 나이아가라 시스템 설정
			TrailEffectComponent->SetAsset(TrailSystem);
			
			// 이펙트 활성화
			if (!TrailEffectComponent->IsActive())
			{
				TrailEffectComponent->Activate(true);
			}
			
			// 발사체 메시 크기에 맞게 이펙트 스케일 조정 (선택적)
			if (ProjectileMeshComponent && ProjectileMeshComponent->GetStaticMesh())
			{
				FVector MeshBounds = ProjectileMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize();
				float MeshScale = FMath::Max(MeshBounds.X, MeshBounds.Y);
				MeshScale = FMath::Max(MeshScale, MeshBounds.Z);
				
				// 메시 크기에 따라 이펙트 스케일 조정
				TrailEffectComponent->SetRelativeScale3D(FVector(1.0f) * (MeshScale / 100.0f));
			}
		}
	}
}

void AProjectile::MulticastSpawnImpactEffect_Implementation(const FVector& Location, const FRotator& Rotation)
{
	// 충돌 이펙트 재생 (서버와 모든 클라이언트에서 실행됨)
	if (!ImpactEffect.IsNull())
	{
		UNiagaraSystem* Effect = ImpactEffect.LoadSynchronous();
		if (Effect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				Effect,
				Location,
				Rotation
			);
			
		}
	}

	// 충돌 효과음 재생 (서버와 모든 클라이언트에서 실행됨)
	if (!ImpactSound.IsNull())
	{
		USoundBase* Sound = ImpactSound.LoadSynchronous();
		if (Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Sound, Location);
		}
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 사용자 정의 회전이 활성화되어 있고, 회전률이 0이 아닌 경우에만 적용
	if (!bRotationFollowsVelocity && !CustomRotationRate.IsZero())
	{
		// 현재 회전에 사용자 정의 회전 속도를 적용
		FRotator CurrentRotation = GetActorRotation();
		FRotator AddRotation = CustomRotationRate * DeltaTime;

		// 새 회전 설정
		SetActorRotation(CurrentRotation + AddRotation);
	}
}

void AProjectile::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    // 에디터에서 스폰된 경우 자동 설정
    if (ProjectileMesh)
    {
        ProjectileMeshComponent->SetStaticMesh(ProjectileMesh);
    }
}

void AProjectile::LaunchFromInstigator()
{
    if (!ProjectileMovement)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to launch projectile - ProjectileMovement is null"), *GetName());
        return;
    }
    
    // 인스티게이터(발사자) 확인
    APawn* InstigatorPawn = GetInstigator();
    if (!InstigatorPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: No instigator found for projectile, using actor forward vector"), *GetName());
        // 인스티게이터가 없으면 액터 자체의 전방 벡터 사용
        FireProjectile(GetActorForwardVector());
        return;
    }
    
    // 먼저 컨트롤러를 확인
    AController* InstigatorController = InstigatorPawn->GetController();
    FVector LaunchDirection;
    
    if (InstigatorController)
    {
        // 컨트롤러가 있으면(플레이어 또는 AI) 컨트롤러의 제어 회전 방향 사용
        FRotator ControlRotation = InstigatorController->GetControlRotation();
        LaunchDirection = ControlRotation.Vector();
    }
    else
    {
        // 컨트롤러가 없으면 폰의 전방 벡터 사용
        LaunchDirection = InstigatorPawn->GetActorForwardVector();
    }
    
    // 발사체의 시작 위치를 조정 (캐릭터의 중앙보다 약간 앞으로)
    FVector AdjustedLocation = GetActorLocation() + LaunchDirection * 50.0f;
    SetActorLocation(AdjustedLocation);
    
    // 전방 벡터 방향으로 발사
    FireProjectile(LaunchDirection);
    
    UE_LOG(LogTemp, Verbose, TEXT("%s: Auto-launched from instigator with direction: %s"), 
           *GetName(), *LaunchDirection.ToString());
}


void AProjectile::FireProjectile(const FVector& Direction)
{
	if (ProjectileMovement)
	{
		// 발사체 소유자 (캐릭터) 가져오기
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetInstigator());
		FVector ForwardDirection;
        
		if (OwnerCharacter)
		{
			// 캐릭터가 바라보는 방향 가져오기 (전방 벡터)
			ForwardDirection = OwnerCharacter->GetActorForwardVector();
		}
		else
		{
			// 소유자가 없거나 캐릭터가 아닌 경우 주어진 방향 사용
			ForwardDirection = Direction.GetSafeNormal();
		}
        
		// 디버그 출력 추가
		UE_LOG(LogTemp, Warning, TEXT("Firing projectile in character's forward direction: %s, Speed: %f"),
			   *ForwardDirection.ToString(), InitialSpeed);

		// 발사체 회전 설정 - 캐릭터가 바라보는 방향
		SetActorRotation(ForwardDirection.Rotation());

		// 속도 설정 - 캐릭터가 바라보는 방향으로
		ProjectileMovement->Velocity = ForwardDirection * InitialSpeed;

		// 이동 컴포넌트 활성화 확인
		ProjectileMovement->SetActive(true);
		ProjectileMovement->SetUpdatedComponent(ProjectileMeshComponent);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// 통합 충돌 처리 함수로 위임
	HandleImpact(OtherActor, Hit);
}

void AProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 통합 충돌 처리 함수로 위임
	HandleImpact(OtherActor, SweepResult);
}

void AProjectile::HandleImpact(AActor* OtherActor, const FHitResult& Hit)
{
	// 유효하지 않은 충돌 무시
	if (!OtherActor || OtherActor == GetOwner() || OtherActor == GetInstigator())
	{
		return;
	}

	// 이미 충돌한 타겟은 무시
	if (!AddHitTarget(OtherActor))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("OtherActor : %s"), *OtherActor->GetName());
	// 충돌 시간 타임스탬프 확인 (매우 짧은 시간 내 중복 충돌 방지)
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastImpactTime < 0.05f) // 50ms 간격
	{
		// 이전 충돌로부터 너무 짧은 시간 내에 발생한 충돌은 무시
		return;
	}
	LastImpactTime = CurrentTime;

	// 데미지 적용
	ApplyDamage(OtherActor, Hit);

	// 이펙트 스폰
	SpawnImpactEffect(Hit);
	
	// 델리게이트 호출 (이벤트 브로드캐스트)
	OnProjectileHit.Broadcast(OtherActor);

	// 최대 관통 횟수에 도달하면 발사체 제거
	if (!bIsPiercing || HitTargets.Num() >= MaxPierceCount)
	{
		Destroy();
	}
}

bool AProjectile::AddHitTarget(AActor* Target)
{
	if (!Target)
	{
		return false;
	}

	// 대상이 이미 히트 목록에 존재하는지 확인
	if (HitTargets.Contains(Target))
	{
		return false;
	}

	// 없으면 추가하고 true 반환
	HitTargets.Add(Target);
	return true;
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 트레일 이펙트 중지 (바로 사라지지 않고 페이드아웃)
	if (TrailEffectComponent && TrailEffectComponent->IsActive())
	{
		// 이펙트의 부드러운 종료를 위해 Deactivate 호출
		TrailEffectComponent->Deactivate();
	}

	// 소멸 이벤트 브로드캐스트
	OnProjectileDestroyed.Broadcast(this);
	
	// 부모 구현 호출
	Super::EndPlay(EndPlayReason);
}

void AProjectile::ApplyDamage(AActor* TargetActor, const FHitResult& Hit)
{
	if (!TargetActor)
	{
		return;
	}

	// 대상이 캐릭터인 경우에만 처리
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	if (!TargetCharacter)
	{
		return;
	}

	// 넉백 방향 계산 - 발사체 진행 방향을 기준으로 함
	FVector KnockbackDirection;
	
	if (ProjectileMovement && !ProjectileMovement->Velocity.IsNearlyZero())
	{
		// 발사체의 현재 속도 방향으로 넉백
		KnockbackDirection = ProjectileMovement->Velocity.GetSafeNormal();
	}
	else 
	{
		// 발사체 -> 타겟 방향으로 넉백
		KnockbackDirection = (TargetCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	}
	
	// 약간 위쪽으로 넉백 방향 조정 (점프 효과)
	KnockbackDirection.Z = FMath::Max(KnockbackDirection.Z, 0.2f);

	// 데미지 시스템 인터페이스 확인 및 데미지 적용
	if (auto DamageTarget = Cast<IDamageSystemUser>(TargetActor))
	{
		// 효과 배열 생성
		TArray<EBehaviorEffect> BehaviorEffects;
		TArray<float> BehaviorEffectsDurations;
		
		// 효과 적용이 활성화된 경우만 추가
		if (bApplyEffect)
		{
			BehaviorEffects.Add(MainEffect);
			BehaviorEffectsDurations.Add(EffectDuration);
		}
		
		// 비어있는 NonBehaviorEffects 및 Duration 배열 생성
		TArray<EBehaviorEffect> NonBehaviorEffects;
		TArray<float> NonBehaviorEffectsDurations;
		
		// 데미지 구조체 구성
		FAttackData AttackData;
		AttackData.Attacker = GetInstigator();
		AttackData.Damage = Damage;
		AttackData.LaunchVector = KnockbackDirection * KnockbackPower;
		AttackData.BehaviorEffects = BehaviorEffects;
		AttackData.BehaviorEffectsDurations = BehaviorEffectsDurations;
		AttackData.NonBehaviorEffects = NonBehaviorEffects;
		AttackData.NonBehaviorEffectsDurations = NonBehaviorEffectsDurations;
		
		// 데미지 적용
		DamageTarget->OnAttacked(AttackData);
	}
}

void AProjectile::SpawnImpactEffect(const FHitResult& Hit)
{
	if (HasAuthority())
	{
		MulticastSpawnImpactEffect(Hit.Location, Hit.Normal.Rotation());
	}
}