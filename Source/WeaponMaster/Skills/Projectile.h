// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class WEAPONMASTER_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
    
	// 발사체 이동 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;
    
	// 발사체 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ProjectileMesh;
    
	// 발사체 발사 메서드
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Launch(FVector Direction, float Speed);
    
	// 충돌 처리
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			  FVector NormalImpulse, const FHitResult& Hit);
    
	// 발사체 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Damage;
};
