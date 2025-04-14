#include "BaseSkill.h"
#include "WeaponMaster/Characters/TestCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "BaseBehaviors/MouseHoverBehavior.h"
#include "Characters/Components/IBattleSystemUser.h"
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
    
    // 기본 공격 속도 및 데미지 추가
    DefaultAttackSpeed = 1.0f;
    DefaultBaseDamage = 0.0f;
}

void UBaseSkill::Initialize(ACharacter* Owner, UItemDataAsset* OwnerItem)
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
    // 기존 코드 (쿨다운 체크 등)
    
    // 스킬 활성화 상태로 변경
    bIsActive = true;
    SkillTimer = 0.0f;
    
    // 스킬 몽타주 재생 (비동기 로드 후)
    if (!SkillMontage.IsNull())
    {
        UAnimMontage* Montage = SkillMontage.LoadSynchronous();
        if (Montage)
        {
            // 재생 속도 계산
            float PlayRate = ItemData ? ItemData->AttackSpeed : DefaultAttackSpeed;
            
            // IBattleSystemUser 인터페이스를 통해 몽타주 재생
            if (OwnerCharacter && OwnerCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
            {
                IBattleSystemUser::Execute_PlaySkillMontage(OwnerCharacter, Montage, PlayRate);
                
                // 몽타주 종료 델리게이트는 여전히 필요 (서버에서만)
                if (OwnerCharacter->HasAuthority())
                {
                    USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh();
                    if (OwnerMesh && OwnerMesh->GetAnimInstance())
                    {
                        FOnMontageEnded EndDelegate;
                        EndDelegate.BindUObject(this, &UBaseSkill::OnMontageEnded);
                        OwnerMesh->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate, Montage);
                    }
                }
            }
        }
    }
    
    // 스킬 로직 실행
    ExecuteSkill();
    
    // 쿨다운 설정
    RemainingCooldown = CooldownTime;
    
    OnSkillStarted.Broadcast(this);
    
    return true;
}

/**
 * 스킬 실행을 수행합니다.
 */
void UBaseSkill::ExecuteSkill()
{
   
}

void UBaseSkill::EndSkill()
{
    UE_LOG(LogTemp, Warning, TEXT("[UBaseSkill::EndSkill] 스킬 종료 브로드캐스트 시작: %s"), *SkillName);
    bIsActive = false;
    OnSkillEnded.Broadcast(this);
    UE_LOG(LogTemp, Warning, TEXT("[UBaseSkill::EndSkill] 브로드캐스트 완료"));
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
    else
    {
        // ItemData가 없으면 기본 공격 속도 사용
        CooldownRate = DefaultAttackSpeed;
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

// 아이템에서 기본 데미지 가져오는 헬퍼 함수 추가
float UBaseSkill::GetItemBaseDamage() const
{
    if (ItemData)
    {
        return ItemData->BaseDamage;
    }
    return DefaultBaseDamage;
}

// 아이템에서 공격 속도 가져오는 헬퍼 함수 추가
float UBaseSkill::GetItemAttackSpeed() const
{
    if (ItemData)
    {
        return ItemData->AttackSpeed;
    }
    return DefaultAttackSpeed;
}

void UBaseSkill::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
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
