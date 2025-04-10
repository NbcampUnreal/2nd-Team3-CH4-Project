// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponMaster/Data/StatusTypes.h"
#include "Projectile.generated.h"

class UNiagaraSystem;

/**
 * 발사체 기본 클래스 - 원거리 스킬 및 투사체 공격에 사용
 */
UCLASS()
class WEAPONMASTER_API AProjectile : public AActor
{
    GENERATED_BODY()

public:
    // 기본 생성자
    AProjectile();

protected:
    // 게임 시작시 호출
    virtual void BeginPlay() override;
    
    // 틱 함수
    virtual void Tick(float DeltaTime) override;

    // 충돌 처리
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
              FVector NormalImpulse, const FHitResult& Hit);
    
    // 오버랩 처리 (피어싱 투사체용)
    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                  bool bFromSweep, const FHitResult& SweepResult);

    // 타겟 배열에 대상 추가 (중복 방지)
    void AddHitTarget(AActor* Target);

    // 데미지 적용 함수
    virtual void ApplyDamage(AActor* TargetActor, const FHitResult& Hit);

    // 이펙트 스폰 함수
    void SpawnImpactEffect(const FHitResult& Hit);

public:
     
    // 발사체 발사 함수
    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void FireProjectile(const FVector& Direction);

protected:
    // 발사체 메시 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ProjectileMeshComponent;
    
    // 발사체 이동 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UProjectileMovementComponent* ProjectileMovement;
    
    // 이미 충돌한 타겟 목록 (피어싱 투사체에서 동일 대상 중복 타격 방지)
    UPROPERTY(Transient)
    TArray<AActor*> HitTargets;

public:
    // 발사체 메시 (생성시 설정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals", meta = (ExposeOnSpawn = "true"))
    UStaticMesh* ProjectileMesh;
    
    // 충돌 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    TSoftObjectPtr<UNiagaraSystem> ImpactEffect;
    
    // 발사체 효과음
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TSoftObjectPtr<USoundBase> ProjectileSound;
    
    // 충돌 효과음
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TSoftObjectPtr<USoundBase> ImpactSound;
    
    // 초기 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ExposeOnSpawn = "true"))
    float InitialSpeed = 3000.0f;
    
    // 최대 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ExposeOnSpawn = "true"))
    float MaxSpeed = 3000.0f;
    
    // 중력 영향도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ExposeOnSpawn = "true"))
    float GravityScale = 0.0f;
    
    // 피어싱 투사체 여부 (관통)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior", meta = (ExposeOnSpawn = "true"))
    bool bIsPiercing = false;
    
    // 관통 가능 횟수 (피어싱 활성화된 경우)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior", meta = (ExposeOnSpawn = "true", EditCondition="bIsPiercing", ClampMin="1"))
    int32 MaxPierceCount = 3;
    
    // 발사체 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (ExposeOnSpawn = "true"))
    float Damage = 20.0f;
    
    // 넉백 파워
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (ExposeOnSpawn = "true"))
    float KnockbackPower = 0.0f;
    
    // CC 효과 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (ExposeOnSpawn = "true"))
    ECCSkillCategory CCEffect = ECCSkillCategory::None;
    
    // CC 효과 지속 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (ExposeOnSpawn = "true"))
    float CCDuration = 0.0f;

    // 발사체 회전 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ExposeOnSpawn = "true"))
    bool bRotationFollowsVelocity = true;

    // 회전 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ExposeOnSpawn = "true", EditCondition="!bRotationFollowsVelocity"))
    FRotator CustomRotationRate = FRotator(0.0f, 0.0f, 720.0f); 
    
};