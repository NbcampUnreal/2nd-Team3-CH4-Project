#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h" 
#include "WeaponMaster/Data/SkillGroupArray.h"
#include "SkillComponent.generated.h"

class USkillDataAsset;
class UBaseSkill;
class UItemDataAsset;
class ATestCharacter;

// 스킬 장착 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillsUpdated, const TArray<UBaseSkill*>&, Skills);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WEAPONMASTER_API USkillComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USkillComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 스킬 컴포넌트만 사용하는경우
    UFUNCTION(BlueprintCallable, Category = "Skills")
    void InitializeSkills(USkillDataAsset* NewSkill);
    
    /**
     * 아이템에서 스킬 초기화
     * @param NewItem 새로 장착된 아이템
     */
    UFUNCTION(BlueprintCallable, Category = "Skills")
    void InitializeSkillsFromItem(UItemDataAsset* NewItem);
    
    /**
     * 스킬 그룹 실행
     * @param GroupIndex 실행할 스킬 그룹 인덱스
     * @return 스킬 실행 성공 여부
     */
    UFUNCTION(BlueprintCallable, Category = "Skills")
    bool ExecuteSkill(int32 GroupIndex);
    
    /**
     * 스킬 ID로 스킬 실행
     * @param SkillID 실행할 스킬 ID
     * @return 스킬 실행 성공 여부
     */
    UFUNCTION(BlueprintCallable, Category = "Skills")
    bool ExecuteSkillByID(FName SkillID);
    
    /**
     * 전체 스킬 목록 반환
     * @return 스킬 배열
     */
    UFUNCTION(BlueprintPure, Category = "Skills")
    TArray<UBaseSkill*> GetSkills() const { return Skills; }
    
    /**
     * 특정 인덱스의 스킬 반환
     * @param SkillIndex 스킬 인덱스
     * @return 해당 인덱스의 스킬 객체
     */
    UFUNCTION(BlueprintPure, Category = "Skills")
    UBaseSkill* GetSkill(int32 SkillIndex) const;
    
    /**
     * 특정 ID의 스킬 반환
     * @param SkillID 스킬 ID
     * @return 해당 ID의 스킬 객체
     */
    UFUNCTION(BlueprintPure, Category = "Skills")
    UBaseSkill* GetSkillByID(FName SkillID) const;
    
    /**
     * 특정 스킬의 인덱스 반환
     * @param SkillID 스킬 ID
     * @return 스킬 인덱스 (없으면 -1)
     */
    UFUNCTION(BlueprintPure, Category = "Skills")
    int32 GetSkillIndex(FName SkillID) const;
    
    /**
     * 스킬 쿨다운 초기화
     * @param SkillIndex 스킬 인덱스
     */
    UFUNCTION(BlueprintCallable, Category = "Skills")
    void ResetSkillCooldown(int32 SkillIndex);
    
    /**
     * 모든 스킬 쿨다운 초기화
     */
    UFUNCTION(BlueprintCallable, Category = "Skills")
    void ResetAllSkillCooldowns();
    
    /**
     * 특정 스킬의 쿨다운 진행률 반환
     * @param SkillIndex 스킬 인덱스
     * @return 쿨다운 진행률 (0.0-1.0)
     */
    UFUNCTION(BlueprintPure, Category = "Skills")
    float GetSkillCooldownProgress(int32 SkillIndex) const;
    
    /**
     * 특정 스킬의 남은 쿨다운 시간 반환
     * @param SkillIndex 스킬 인덱스
     * @return 남은 쿨다운 시간
     */
    UFUNCTION(BlueprintPure, Category = "Skills")
    float GetSkillRemainingCooldown(int32 SkillIndex) const;
    
    // 스킬 업데이트 이벤트
    UPROPERTY(BlueprintAssignable, Category = "Skills|Events")
    FOnSkillsUpdated OnSkillsUpdated;

    /**
     * 현재 활성화된 스킬을 반환합니다.
     * @return 현재 활성화된 스킬 객체. 없으면 nullptr 반환
     */
    UFUNCTION(BlueprintPure, Category = "Skills")
    UBaseSkill* GetActiveSkill() const;
    
protected:
    /**
     * 스킬 생성 및 초기화
     * @param SkillClass 생성할 스킬 클래스
     * @param OwnerItem 스킬이 속한 아이템
     * @return 생성된 스킬 객체
     */
    UFUNCTION()
    UBaseSkill* CreateSkill(TSubclassOf<UBaseSkill> SkillClass, UItemDataAsset* OwnerItem);
    
    /**
     * 스킬 초기화 관련 서버 RPC
     * @param NewItem 새로 장착된 아이템
     */
    UFUNCTION(Server, Reliable)
    void Server_InitializeSkills(UItemDataAsset* NewItem);

    /**
     * 스킬 데이터 애셋에서 초기화 관련 서버 RPC
     * @param NewSkill 새로 설정할 스킬 데이터 애셋
     */
    UFUNCTION(Server, Reliable)
    void Server_InitializeSkillsFromDataAsset(USkillDataAsset* NewSkill);
    
    /**
     * 스킬 실행 서버 RPC
     * @param GroupIndex 실행할 스킬 그룹 인덱스
     */
    UFUNCTION(Server, Reliable)
    void Server_ExecuteSkill(int32 GroupIndex);
    
    /**
     * 스킬 목록 클라이언트 업데이트
     * @param SkillDataArray 스킬 데이터 직렬화 배열
     */
    UFUNCTION(Client, Reliable)
    void Client_UpdateSkills(const TArray<UBaseSkill*>& UpdatedSkills);

    
    /**
     * 그룹 내에서 다음 사용할 스킬 인덱스를 결정
     * @param GroupIndex 스킬 그룹 인덱스
     * @return 다음 사용할 스킬의 그룹 내 인덱스
     */
    UFUNCTION()
    int32 GetNextSkillIndexInGroup(int32 GroupIndex);

    // 쿨다운 업데이트 함수
    UFUNCTION()
    void UpdateSkillCooldowns();
    
    // 소유 캐릭터 캐싱
    UPROPERTY()
    ACharacter* OwnerCharacter;
    
    // 현재 장착된 스킬 목록
    UPROPERTY(Replicated)
    TArray<UBaseSkill*> Skills;
    
    // 스킬 그룹 구조 캐싱 (아이템에 의존하지 않게)
    UPROPERTY()
    TArray<FSkillGroupArray> CachedSkillGroups;
    
    // 현재 장착된 아이템 캐싱
    UPROPERTY()
    UItemDataAsset* CachedItemData;
    
    // 각 그룹별 마지막으로 사용한 스킬 인덱스
    UPROPERTY(Transient)
    TArray<int32> LastUsedSkillIndices;
    
    // 각 그룹별 리셋 준비 상태
    UPROPERTY(Transient)
    TArray<bool> GroupReadyToReset;
    
    // 각 그룹별 스킬 시작 인덱스 캐싱 (Skills 배열에서의 오프셋)
    UPROPERTY(Transient)
    TArray<int32> GroupSkillOffsets;
    
    // 스킬 업데이트를 위한 타이머 핸들
    FTimerHandle CooldownUpdateTimerHandle;

    // 각 그룹별 기회 시간 타이머
    UPROPERTY(Transient)
    TArray<float> OpportunityTimers;

    // 각 그룹이 기회 시간 모드인지 여부
    UPROPERTY(Transient)
    TArray<bool> IsInOpportunityWindow;

    // 기회 시간 지속 시간 (초 단위)
    UPROPERTY(EditAnywhere, Category = "Skills|Config", meta = (ClampMin = "0.0", UIMin = "0.0"))
    float OpportunityWindowDuration;
    
    
};