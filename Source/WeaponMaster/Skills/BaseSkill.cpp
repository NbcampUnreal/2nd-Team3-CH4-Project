#include "BaseSkill.h"
#include "WeaponMaster/Characters/TestCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "WeaponMaster/Data/StatusTypes.h"

UBaseSkill::UBaseSkill()
{
    // 기본값 설정
    SkillID = NAME_None;
    SkillName = TEXT("Unknown Skill");
    Description = TEXT("No description available.");
    SkillType = ESkillType::Melee;
    CooldownTime = 1.0f;
    SkillDuration = 0.0f;
    SkillDamage = 10.0f;
    CCEffect = ECCSkillCategory::None;
    CCDuration = 0.0f;
    RemainingCooldown = 0.0f;
    bIsActive = false;
    SkillTimer = 0.0f;
}

void UBaseSkill::Initialize(ATestCharacter* Owner, UItemDataAsset* OwnerItem)
{
    OwnerCharacter = Owner;
    ItemData = OwnerItem;
}

/**
 * 스킬을 활성화합니다.
 *
 * 스킬 활성화 시, 쿨다운 상태, 소유자 캐릭터 유효성, 스킬 활성화 상태 등을 확인하고,
 * 스킬 몽타주 재생 및 스킬 로직 실행을 수행합니다. 쿨다운 시간이 설정되며,
 * 스킬이 정상적으로 실행되었는지를 반환합니다.
 *
 * @return 스킬 활성화 성공 여부. 활성화에 실패할 경우 false, 성공하면 true를 반환합니다.
 */
bool UBaseSkill::ActivateSkill()
{
    // 쿨다운 체크
    if (!IsSkillReady())
    {
        return false;
    }
    
    // 이미 활성화된 스킬이면 중복 실행 방지
    if (bIsActive)
    {
        return false;
    }
    
    // 소유자 캐릭터 검증
    if (!OwnerCharacter)
    {
        return false;
    }
    
    // 스킬 활성화 상태로 변경
    bIsActive = true;
    SkillTimer = 0.0f;
    
    // 스킬 몽타주 재생 (비동기 로드 후)
    if (!SkillMontage.IsNull())
    {
        UAnimMontage* Montage = SkillMontage.LoadSynchronous();
        if (Montage)
        {
            // 소유자 메시 가져오기
            USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh();
            if (OwnerMesh)
            {
                // 아이템 AttackSpeed에 따른 재생 속도 계산
                float PlayRate = 1.0f;
                if (ItemData)
                {
                    // 아이템의 AttackSpeed를 몽타주 재생 속도에 적용
                    PlayRate = ItemData->AttackSpeed;
                }
                
                // 애니메이션 몽타주 재생 (PlayRate 적용)
                float MontageLength = OwnerMesh->GetAnimInstance()->Montage_Play(Montage, PlayRate);
                if (MontageLength > 0.0f)
                {
                    // 몽타주 종료 시 호출될 델리게이트 바인딩
                    FOnMontageEnded EndDelegate;
                    EndDelegate.BindUObject(this, &UBaseSkill::OnMontageEnded);
                    OwnerMesh->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate, Montage);
                }
            }
        }
    }
    
    // 스킬 로직 실행
    ExecuteSkill();
    
    // 쿨다운 설정
    RemainingCooldown = CooldownTime;
    
    return true;
}

/**
 * 스킬 실행을 수행합니다.
 *
 * 기본적으로 스킬 사운드와 이펙트를 재생하며, 자식 클래스에서 오버라이드하여
 * 스킬의 세부적인 로직을 구현할 수 있습니다. 스킬의 소유자 캐릭터와 관련된
 * 위치 및 회전을 참조하여 사운드와 이펙트를 실행합니다.
 *
 * - 스킬 사운드: "SkillSound" 변수에 설정된 TSoftObjectPtr<USoundBase>를 로드하여
 *   소유자의 위치에서 재생합니다. 사운드 데이터가 없거나 로드 실패 시, 사운드는 재생되지 않습니다.
 *
 * - 스킬 이펙트: "SkillEffect" 변수에 설정된 TSoftObjectPtr<UNiagaraSystem>을 로드하여
 *   소유자의 위치 및 회전에 따라 이펙트를 스폰합니다. 이펙트 데이터가 없거나 로드 실패 시, 이펙트는 생성되지 않습니다.
 *
 * 이 메서드의 기본 구현은 각 스킬의 공통적인 시각 및 청각 효과를 담당하며,
 * 실제 개별 스킬의 동작은 오버라이드된 메서드에서 더 구체화됩니다.
 */
void UBaseSkill::ExecuteSkill()
{
    
}

void UBaseSkill::EndSkill()
{
    // 스킬 비활성화
    bIsActive = false;
}

void UBaseSkill::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // 몽타주가 끝나면 스킬 종료
    EndSkill();
}

bool UBaseSkill::IsSkillReady() const
{
    return RemainingCooldown <= 0.0f;
}

bool UBaseSkill::IsSkillActive() const
{
    return bIsActive;
}

void UBaseSkill::UpdateCooldown(float DeltaTime)
{
    // 아이템 AttackSpeed에 따른 쿨다운 감소 속도 계산
    float CooldownRate = 1.0f;
    if (ItemData)
    {
        // 아이템의 AttackSpeed를 쿨다운 감소 속도에 적용
        CooldownRate = ItemData->AttackSpeed;
    }
    
    // 쿨다운 업데이트 (공격 속도에 따라 가속)
    if (RemainingCooldown > 0.0f)
    {
        RemainingCooldown = FMath::Max(0.0f, RemainingCooldown - (DeltaTime * CooldownRate));
    }
    
    // 스킬 타이머 업데이트 (스킬 사용 중인 경우)
    if (bIsActive)
    {
        // 스킬 타이머도 공격 속도에 비례하여 업데이트
        SkillTimer += DeltaTime * CooldownRate;
        
        // 스킬 지속 시간 체크 (지속시간이 0보다 크면 타이머로 종료)
        if (SkillDuration > 0.0f && SkillTimer >= SkillDuration)
        {
            EndSkill();
        }
    }
}

float UBaseSkill::GetCooldownProgress() const
{
    if (CooldownTime <= 0.0f)
    {
        return 1.0f;
    }
    
    return 1.0f - (RemainingCooldown / CooldownTime);
}

int32 UBaseSkill::ProcessTargetActors(const TArray<AActor*>& TargetActors, float Damage)
{
    // 소유자 캐릭터 확인
    if (!OwnerCharacter)
    {
        return 0;
    }
    
    // 스킬이 준비되지 않았거나 이미 활성화 상태라면 처리하지 않음
    if (!IsSkillReady() || bIsActive)
    {
        return 0;
    }
    
    // 처리된 유효한 액터 수
    int32 ValidActorsCount = 0;
    
    // 액터 배열 순회
    for (AActor* TargetActor : TargetActors)
    {
        // 액터 유효성 검사
        if (!TargetActor || TargetActor == OwnerCharacter)
        {
            continue;
        }
        
        // 액터가 유효하면 카운트 증가
        // 자식 클래스에서는 이 부분을 오버라이드하여 실제 스킬 효과 적용 가능
        ValidActorsCount++;
    }
    
    return ValidActorsCount;
}
