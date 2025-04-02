// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponItemBase.h"
#include "GameFramework/Actor.h"
#include "SwordItem.generated.h"

UCLASS()
class WEAPONMASTER_API ASwordItem : public AWeaponItemBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwordItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
