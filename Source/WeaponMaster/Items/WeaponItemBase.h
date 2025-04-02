// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "WeaponItemBase.generated.h"

class UBoxComponent;
// 스킬 구조체
USTRUCT(BlueprintType)
struct FSkillData
{
	GENERATED_BODY()

	// 스킬 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FString SkillName;

	// 스킬 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FString SkillDescription;

	// 스킬 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSoftObjectPtr<UTexture2D> SkillIcon;

	// 스킬 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	UAnimMontage* SkillMontage;

	// 스킬 대미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float Damage;

	// 스킬 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float Cooldown;

	// 스킬 범위(쓸지 안쓸지 모름)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float Range;

	// 기본 생성자
	FSkillData()
		: SkillIcon(nullptr), SkillMontage(nullptr), Damage(10.0f), Cooldown(1.0f), Range(100.0f)
	{
	}
};

UCLASS()
class WEAPONMASTER_API AWeaponItemBase : public AItemBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

public:
	// 무기가 가진 스킬 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<FSkillData> Skills;

	// 무기 장착 소켓 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName AttachSocketName;

	// 공격 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* AttackCollision;

	// 장착 오버라이드
	virtual void OnEquip_Implementation(ACharacter* Character) override;

	// 스킬 사용 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool UseSkill(int32 SkillIndex, ACharacter* Character);

	// 공격 충돌 활성화/비활성화
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EnableAttackCollision();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DisableAttackCollision();

	// 공격 충돌 이벤트
	UFUNCTION()
	void OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	// 현재 스킬 인덱스
	int32 CurrentSkillIndex;
	
	// 현재 쿨타임 상태
	TArray<float> SkillCooldownRemaining;
};
