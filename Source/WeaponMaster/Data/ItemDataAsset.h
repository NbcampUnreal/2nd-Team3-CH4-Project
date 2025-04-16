#pragma once

#include "CoreMinimal.h"
#include "SkillGroupArray.h"
#include "Engine/DataAsset.h"
#include "WeaponMaster/Skills/BaseSkill.h"
#include "NiagaraSystem.h"
#include "ItemDataAsset.generated.h"

/**
 * 아이템의 메시와 이펙트 데이터를 담는 구조체
 */
USTRUCT(BlueprintType)
struct FItemMeshData
{
    GENERATED_BODY()
    
    // 장착 메시
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
    TSoftObjectPtr<UStaticMesh> Mesh;
    
    // 부착할 소켓 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
    FName SocketName;
    
    // 메시 스케일
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
    FVector Scale;
    
    // 이 메시에 적용할 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    TSoftObjectPtr<UNiagaraSystem> Effect;
    
    // 이펙트 위치 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    FVector EffectOffset;
    
    // 이펙트 스케일
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    FVector EffectScale;
    
    // 이펙트 색상 파라미터 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    FName EffectColorParameterName;
    
    // 이펙트 색상
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    FLinearColor EffectColor;
    
    // 기본 생성자
    FItemMeshData()
        : SocketName(TEXT("hand_r"))
        , Scale(FVector(1.0f, 1.0f, 1.0f))
        , EffectOffset(FVector(0.0f, 0.0f, 50.0f))
        , EffectScale(FVector(1.0f, 1.0f, 1.0f))
        , EffectColorParameterName(TEXT("Color"))
        , EffectColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
    {
    }
};

/**
 * 
 */
UCLASS()
class WEAPONMASTER_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UItemDataAsset();
	
	// 아이템 기본 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FName ItemID;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FString ItemName;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info", meta = (MultiLine=true))
	FString Description;
    
	// 아이템 메시 (월드에 드롭되었을 때)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TSoftObjectPtr<UStaticMesh> ItemMesh;
    
	// 여러 메시와 이펙트 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TArray<FItemMeshData> MeshData;
    
	// 기본 공격력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float BaseDamage;
    
	// 공격 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackSpeed;
	
	// 아이콘 (UI 표시용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;
    
	// 획득 효과음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> PickupSound;
    
	// 장착 효과음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> EquipSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skills")
	TArray<FSkillGroupArray> SkillGroups;
	
	// DataAsset 타입 오버라이드
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};