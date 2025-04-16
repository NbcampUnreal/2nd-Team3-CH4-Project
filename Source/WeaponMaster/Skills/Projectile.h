// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponMaster/Data/StatusTypes.h"
#include "Projectile.generated.h"

class UNiagaraSystem;
class UProjectileMovementComponent;

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProjectileHitDelegate, AActor*, HitActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProjectileDestroyedDelegate, AProjectile*, Projectile);

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

    // 발사체 자동 발사 - 인스티게이터의 전방 벡터 방향으로 발사
    void LaunchFromInstigator();
    
    // 발사체 발사 함수 (레거시 지원)
    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void FireProjectile(const FVector& Direction);
    
    // 메시 컴포넌트 게터
    UFUNCTION(BlueprintCallable, Category = "Projectile|Components")
    UStaticMeshComponent* GetProjectileMeshComponent() const { return ProjectileMeshComponent; }
    
    // 프로젝타일 무브먼트 게터
    UFUNCTION(BlueprintCallable, Category = "Projectile|Components")
    UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
    
protected:
    // 게임 시작시 호출
    virtual void BeginPlay() override;
    
    // 에디터에서 액터가 스폰됐을 때 호출
    virtual void PostInitializeComponents() override;
    
    // 틱 함수
    virtual void Tick(float DeltaTime) override;

    // 충돌 처리 통합 함수
    UFUNCTION(BlueprintCallable, Category = "Projectile|Impact")
    virtual void HandleImpact(AActor* OtherActor, const FHitResult& Hit);

    // 충돌 이벤트 핸들러
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
              FVector NormalImpulse, const FHitResult& Hit);
    
    // 오버랩 이벤트 핸들러
    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                  bool bFromSweep, const FHitResult& SweepResult);

    // 타겟 배열에 대상 추가 (중복 방지)
    FORCEINLINE bool AddHitTarget(AActor* Target);
    
    // 발사체 소멸 전 호출
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // 데미지 적용 함수
    virtual void ApplyDamage(AActor* TargetActor, const FHitResult& Hit);

    // 이펙트 스폰 함수
    void SpawnImpactEffect(const FHitResult& Hit);

    // 충돌 설정 초기화
    void InitializeCollision();

    // 무브먼트 설정 초기화
    void InitializeMovement();

    // 시각/사운드 효과 초기화
    void InitializeEffects();

protected:
    // 발사체 메시 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ProjectileMeshComponent;
    
    // 발사체 이동 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProjectileMovementComponent* ProjectileMovement;
    
    // 트레일 이펙트 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UNiagaraComponent* TrailEffectComponent;
    
    // 이미 충돌한 타겟 목록 (피어싱 투사체에서 동일 대상 중복 타격 방지)
    UPROPERTY(Transient)
    TSet<AActor*> HitTargets;
    
    // 마지막 충돌 시간 (다중 충돌 방지)
    UPROPERTY(Transient)
    float LastImpactTime;

    // 메시를 보이게 할지 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals", meta = (ExposeOnSpawn = "true"))
    bool bMeshVisible = false;
public:
    // 발사체 히트 이벤트
    UPROPERTY(BlueprintAssignable, Category = "Projectile|Events")
    FOnProjectileHitDelegate OnProjectileHit;
    
    // 발사체 소멸 이벤트
    UPROPERTY(BlueprintAssignable, Category = "Projectile|Events")
    FOnProjectileDestroyedDelegate OnProjectileDestroyed;
    
    // 발사체 메시
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    UStaticMesh* ProjectileMesh;
    
    // 충돌 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    TSoftObjectPtr<UNiagaraSystem> ImpactEffect;
    
    // 발사체 트레일 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    TSoftObjectPtr<UNiagaraSystem> TrailEffect;
    
    // 발사체 효과음
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TSoftObjectPtr<USoundBase> ProjectileSound;
    
    // 충돌 효과음
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TSoftObjectPtr<USoundBase> ImpactSound;
    
    // 초기 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float InitialSpeed = 3000.0f;
    
    // 최대 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxSpeed = 3000.0f;
    
    // 중력 영향도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float GravityScale = 0.0f;
    
    // 피어싱 투사체 여부 (관통)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bIsPiercing = false;
    
    // 관통 가능 횟수 (피어싱 활성화된 경우)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior", meta = (EditCondition="bIsPiercing", ClampMin="1"))
    int32 MaxPierceCount = 3;
    
    // 발사체 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float Damage = 20.0f;
    
    // 넉백 파워
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float KnockbackPower = 100.0f;
    
    // 행동 효과 타입 (기본값은 없음을 의미)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (EditCondition = "bApplyEffect"))
    EBehaviorEffect MainEffect = EBehaviorEffect::Stiffness;
    
    // 효과 적용 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    bool bApplyEffect = false;
    
    // CC 효과 지속 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float EffectDuration = 0.0f;

    // 발사체 회전 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bRotationFollowsVelocity = true;

    // 회전 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition="!bRotationFollowsVelocity"))
    FRotator CustomRotationRate = FRotator(0.0f, 0.0f, 720.0f);

    // 모든 클라이언트에 이펙트 재생을 알리는 멀티캐스트 RPC
    UFUNCTION(NetMulticast, Reliable)
    void MulticastSpawnImpactEffect(const FVector& Location, const FRotator& Rotation);
};