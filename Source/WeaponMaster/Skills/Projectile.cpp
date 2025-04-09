// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
    
	// 메시 컴포넌트 설정
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	SetRootComponent(ProjectileMesh);
	ProjectileMesh->SetSimulatePhysics(true);
	ProjectileMesh->SetEnableGravity(false); // 중력 비활성화
	ProjectileMesh->SetCollisionProfileName(TEXT("Projectile"));
	ProjectileMesh->SetGenerateOverlapEvents(true);
    
	// 프로젝타일 무브먼트 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 중력 없음
    
	// 데미지 기본값
	Damage = 20.0f;
    
	// 충돌 이벤트 바인딩
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    
	// 수명 설정 (5초 후 자동 삭제)
	InitialLifeSpan = 5.0f;
}

void AProjectile::Launch(FVector Direction, float Speed)
{
	if (ProjectileMovement)
	{
		// 발사 방향 설정
		ProjectileMovement->Velocity = Direction * Speed;
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
						FVector NormalImpulse, const FHitResult& Hit)
{
	// 자기 자신이나 발사자는 공격하지 않음
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}
    
	// 데미지 적용
	UGameplayStatics::ApplyDamage(
		OtherActor,
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);
    
	// 발사체 효과 (폭발 등)
	// ... 효과 코드 ...
    
	// 발사체 제거
	Destroy();
}

