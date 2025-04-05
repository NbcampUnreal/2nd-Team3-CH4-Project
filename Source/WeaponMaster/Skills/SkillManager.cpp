// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillManager.h"
#include "CollisionManager.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "WeaponMaster/Characters/TestCharacter.h"
#include "WeaponMaster/Characters/WeaponMasterCharacter.h"
#include "WeaponMaster/Characters/Components/ItemComponent/ItemComponent.h"

USkillManager::USkillManager()
{
    bCollisionEnabled = false;
    ActiveSkillIndex = -1;
    
    // 충돌 관리자 생성
    CollisionManager = CreateDefaultSubobject<UCollisionManager>(TEXT("CollisionManager"));
}

/**
 * 지정된 스킬 데이터를 등록합니다. 스킬 타입이 None이 아닌 경우에만 등록됩니다.
 *
 * @param SkillData 등록할 스킬 데이터
 */
void USkillManager::RegisterSkill(const FSkillData& SkillData)
{
    if (SkillData.SkillType != ESkillType::None)
    {
        // 인덱스 기반 배열에 저장
        SkillDataArray.Add(SkillData);
    }
}

/**
 * 주어진 스킬 인덱스를 기반으로 스킬을 실행합니다. 실행 조건이 충족되지 않을 경우 스킬은 실행되지 않습니다.
 *
 * @param SkillIndex 실행할 스킬의 인덱스
 * @param Instigator 스킬을 실행하는 주체
 */
void USkillManager::ExecuteSkill(int32 SkillIndex, AActor* Instigator)
{
    // 인덱스 범위 확인
    if (SkillIndex < 0 || SkillIndex >= SkillDataArray.Num() || !Instigator)
    {
        return;
    }
    
    // 스킬 데이터 가져오기
    const FSkillData& SkillData = SkillDataArray[SkillIndex];
    
    // 쿨다운 확인
    if (IsSkillOnCooldown(SkillIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Skill %s is on cooldown"), *SkillData.SkillName);
        return;
    }
    
    // 캐릭터 확인
    ACharacter* Character = Cast<ACharacter>(Instigator);
    if (!Character)
    {
        return;
    }
    
    // 현재 활성화된 스킬 설정
    ActiveSkillIndex = SkillIndex;
    
    // 애니메이션 재생
    PlaySkillAnimation(Instigator, SkillData);
    
    // 쿨다운 적용
    if (GetWorld())
    {
        // 인덱스 배열 크기 확인 및 필요시 확장
        if (CooldownTimerHandles.Num() <= SkillIndex)
        {
            CooldownTimerHandles.SetNum(SkillIndex + 1);
        }
        
        GetWorld()->GetTimerManager().SetTimer(
            CooldownTimerHandles[SkillIndex],
            FTimerDelegate(),
            SkillData.CooldownTime,
            false
        );
    }
    
    // Execute 인터페이스 메서드 호출
    Execute(Instigator, SkillData);
}

/**
 * 인덱스로 스킬 데이터를 가져옵니다.
 * 
 * @param SkillIndex 가져올 스킬의 인덱스
 * @return 해당 인덱스의 스킬 데이터, 유효하지 않은 인덱스인 경우 기본 스킬 데이터 반환
 */
FSkillData USkillManager::GetSkillData(int32 SkillIndex) const
{
    if (SkillIndex >= 0 && SkillIndex < SkillDataArray.Num())
    {
        return SkillDataArray[SkillIndex];
    }
    
    // 유효하지 않은 인덱스인 경우 기본 스킬 데이터 반환
    FSkillData DefaultData;
    DefaultData.SkillType = ESkillType::None;
    DefaultData.SkillName = TEXT("Invalid Skill");
    return DefaultData;
}

/**
 * 인덱스로 스킬이 쿨다운 중인지 확인합니다.
 * 
 * @param SkillIndex 확인할 스킬의 인덱스
 * @return 해당 스킬이 쿨다운 중이면 true, 아니면 false
 */
bool USkillManager::IsSkillOnCooldown(int32 SkillIndex) const
{
    if (SkillIndex < 0 || SkillIndex >= CooldownTimerHandles.Num() || !GetWorld())
    {
        return false;
    }
    
    return GetWorld()->GetTimerManager().IsTimerActive(CooldownTimerHandles[SkillIndex]);
}

/**
 * 주어진 스킬 데이터를 기반으로 스킬을 실행합니다. 서브클래스에서 오버라이드가 가능하며, 공통적인 스킬 동작을 처리합니다.
 *
 * @param Instigator 스킬을 실행하는 액터
 * @param SkillData 실행할 스킬의 데이터
 */
void USkillManager::Execute(AActor* Instigator, const FSkillData& SkillData)
{
    // 기본 구현 - 서브클래스에서 오버라이드 가능
    // 여기에서는 공통적인 스킬 행동만 처리
    
    // 충돌 처리 리셋
    if (CollisionManager)
    {
        CollisionManager->ResetHitRegistry();
    }
    
    // 효과음 재생
    if (!SkillData.SkillSound.IsNull())
    {
        USoundBase* Sound = SkillData.SkillSound.LoadSynchronous();
        if (Sound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                Instigator->GetWorld(),
                Sound,
                Instigator->GetActorLocation()
            );
        }
    }
    
    // 파티클 효과
    if (!SkillData.SkillEffect.IsNull())
    {
        UParticleSystem* Effect = SkillData.SkillEffect.LoadSynchronous();
        if (Effect)
        {
            UGameplayStatics::SpawnEmitterAtLocation(
                Instigator->GetWorld(),
                Effect,
                Instigator->GetActorLocation(),
                Instigator->GetActorRotation()
            );
        }
    }
}

/**
 * 현재 활성화된 스킬이 쿨다운 상태인지 확인합니다.
 *
 * @return 활성화된 스킬이 쿨다운 상태이면 true, 그렇지 않으면 false를 반환합니다.
 */
bool USkillManager::IsOnCooldown() const
{
    if (ActiveSkillIndex >= 0)
    {
        return IsSkillOnCooldown(ActiveSkillIndex);
    }
    
    return false;
}

/**
 * 현재 활성화된 스킬의 남은 쿨다운 시간을 반환합니다.
 * 스킬이 활성화되어 있고 유효한 타이머가 존재할 경우, 남은 쿨다운 시간을 반환합니다.
 * 그렇지 않은 경우 0.0을 반환합니다.
 *
 * @return float 현재 활성화된 스킬의 남은 쿨다운 시간 (초 단위). 유효한 타이머가 없거나 스킬이 활성화되어 있지 않으면 0.0을 반환합니다.
 */
float USkillManager::GetCooldownRemaining() const
{
    if (ActiveSkillIndex >= 0)
    {
        return GetCooldownRemaining(ActiveSkillIndex);
    }
    
    return 0.0f;
}

/**
 * 인덱스로 스킬의 남은 쿨다운 시간을 반환합니다.
 * 스킬이 활성화되어 있고 유효한 타이머가 존재할 경우, 남은 쿨다운 시간을 반환합니다.
 * 그렇지 않은 경우 0.0을 반환합니다.
 *
 * @param SkillIndex 확인할 스킬의 인덱스
 * @return float 지정된 스킬의 남은 쿨다운 시간 (초 단위)
 */
float USkillManager::GetCooldownRemaining(int32 SkillIndex) const
{
    if (SkillIndex < 0 || SkillIndex >= CooldownTimerHandles.Num() || !GetWorld())
    {
        return 0.0f;
    }
    
    return GetWorld()->GetTimerManager().GetTimerRemaining(CooldownTimerHandles[SkillIndex]);
}

/**
 * 지정된 쿨다운 시간을 적용합니다. 유효한 시간이 제공되지 않거나 월드 컨텍스트가 없는 경우 동작하지 않습니다.
 *
 * @param CooldownTime 적용할 쿨다운 시간(초 단위)
 */
void USkillManager::ApplyCooldown(float CooldownTime)
{
    if (CooldownTime <= 0.0f || !GetWorld() || ActiveSkillIndex < 0)
    {
        return;
    }
    
    // 인덱스 배열 크기 확인 및 필요시 확장
    if (CooldownTimerHandles.Num() <= ActiveSkillIndex)
    {
        CooldownTimerHandles.SetNum(ActiveSkillIndex + 1);
    }
    
    // 쿨다운 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(
        CooldownTimerHandles[ActiveSkillIndex],
        FTimerDelegate(),
        CooldownTime,
        false
    );
}

/**
 * 무기 충돌 발생 시 처리 작업을 수행합니다.
 * 충돌한 액터의 데미지 처리와 상태 이상을 적용합니다.
 *
 * @param Weapon 충돌한 무기 컴포넌트
 * @param HitActor 충돌한 액터
 * @param HitResult 충돌 결과 정보
 */
void USkillManager::ProcessWeaponCollision(UPrimitiveComponent* Weapon, AActor* HitActor, const FHitResult& HitResult)
{
    // 충돌 관리자를 통한 충돌 처리
    if (!CollisionManager || !HitActor || !bCollisionEnabled)
    {
        return;
    }
    
    // 중복 히트 체크
    if (!CollisionManager->RegisterHit(HitActor))
    {
        return; // 이미 히트한 액터
    }
    
    // 현재 활성화된 스킬 데이터 가져오기
    if (ActiveSkillIndex < 0 || ActiveSkillIndex >= SkillDataArray.Num())
    {
        return;
    }
    
    const FSkillData& SkillData = SkillDataArray[ActiveSkillIndex];
    
    // 데미지 계산
    float TotalDamage = SkillData.Damage;
    
    // 데미지 적용
    UItemComponent* ItemComp = Cast<UItemComponent>(GetOuter());
    if (ItemComp)
    {
        ATestCharacter* Character = Cast<ATestCharacter>(ItemComp->GetOwner());
        if (Character)
        {
            // 데미지 이벤트 생성
            TSubclassOf<UDamageType> DamageTypeClass = SkillData.DamageType;
            if (!DamageTypeClass)
            {
                DamageTypeClass = UDamageType::StaticClass();
            }
            
            // 서버에 데미지 적용 요청
            Character->Server_ApplyDamage(HitActor, TotalDamage, DamageTypeClass);
            
        }
    }
}

/**
 * 주어진 스킬 데이터를 기반으로 스킬 애니메이션을 실행합니다.
 * 스킬 애니메이션이 없거나 유효하지 않을 경우, 또는 Instigator가 캐릭터가 아닌 경우 실행되지 않습니다.
 *
 * @param Instigator 스킬을 실행하는 주체 (AActor 타입)
 * @param SkillData 실행할 스킬에 대한 정보 (FSkillData 구조체)
 */
void USkillManager::PlaySkillAnimation(AActor* Instigator, const FSkillData& SkillData)
{
    // 애니메이션이 없으면 무시
    if (SkillData.SkillAnimation.IsNull())
    {
        return;
    }
    
    // 캐릭터 확인
    ACharacter* Character = Cast<ACharacter>(Instigator);
    if (!Character)
    {
        return;
    }
    
    // 애니메이션 몽타주 로드
    UAnimMontage* AnimMontage = SkillData.SkillAnimation.LoadSynchronous();
    if (!AnimMontage)
    {
        return;
    }
    
    // 애니메이션 인스턴스 가져오기
    USkeletalMeshComponent* MeshComp = Character->GetMesh();
    if (!MeshComp)
    {
        return;
    }
    
    UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
    if (!AnimInstance)
    {
        return;
    }
    
    // 몽타주 재생
    float Duration = AnimInstance->Montage_Play(AnimMontage);
    
    // 애님 노티파이 델리게이트 바인딩
    //현재는 바인딩 하는 함수가 없음
    FOnMontageEnded MontageEndedDelegate;
    
    
    AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AnimMontage);
}
