// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CollisionManager.generated.h"

UCLASS()
class WEAPONMASTER_API UCollisionManager : public UObject
{
	GENERATED_BODY()
    
public:
	UCollisionManager();
    
	/** 히트 액터 등록 (중복 히트 방지) */
	UFUNCTION(BlueprintCallable, Category = "Collision")
	bool RegisterHit(AActor* HitActor);
    
	/** 히트 레지스트리 초기화 */
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void ResetHitRegistry();
    
	/** 무기 충돌 처리 */
	UFUNCTION(BlueprintCallable, Category = "Collision")
	bool ProcessWeaponCollision(UPrimitiveComponent* Weapon, const FHitResult& HitResult);
    
private:
	/** 이미 히트한 액터 목록 */
	UPROPERTY()
	TSet<AActor*> HitActors;
};