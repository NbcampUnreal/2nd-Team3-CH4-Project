// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionManager.h"

UCollisionManager::UCollisionManager()
{
	// 초기화
	HitActors.Empty();
}

bool UCollisionManager::RegisterHit(AActor* HitActor)
{
	if (!HitActor)
	{
		return false;
	}
    
	// 이미 등록된 액터인지 확인
	if (HitActors.Contains(HitActor))
	{
		return false;
	}
    
	// 새 히트 액터 등록
	HitActors.Add(HitActor);
	return true;
}

void UCollisionManager::ResetHitRegistry()
{
	// 히트 레지스트리 초기화
	HitActors.Empty();
}

bool UCollisionManager::ProcessWeaponCollision(UPrimitiveComponent* Weapon, const FHitResult& HitResult)
{
	if (!Weapon || !HitResult.GetActor())
	{
		return false;
	}
    
	// 히트 액터 등록
	return RegisterHit(HitResult.GetActor());
}