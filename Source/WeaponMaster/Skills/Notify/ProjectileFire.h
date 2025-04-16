

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ProjectileFire.generated.h" 

class AActor;

/**
 * 발사체를 특정 위치에 스폰하는 AnimNotify 클래스입니다.
 * 클래스 이름이 UProjectileFire로 변경되었습니다.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Projectile Fire Notify")) 
class WEAPONMASTER_API UProjectileFire : public UAnimNotify 
{
	GENERATED_BODY()

public:
	UProjectileFire();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	FVector SpawnLocationOffset = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true", EditCondition = "bDrawDebug"))
	FLinearColor DebugLineColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true", EditCondition = "bDrawDebug", ClampMin = "0.0"))
	float DebugSphereRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true", EditCondition = "bDrawDebug", ClampMin = "0.0"))
	float DebugDuration = 10.0f; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true", EditCondition = "bDrawDebug", ClampMin = "3"))
	int32 DebugSegments = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true", EditCondition = "bDrawDebug", ClampMin = "0.0"))
	float DebugThickness = 0.0f;

};