#include "BaseSkill.h"

#include "NiagaraFunctionLibrary.h"
#include "WeaponMaster/Characters/TestCharacter.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/DamageSystemUser.h"
#include "WeaponMaster/Data/StatusTypes.h"
#include "NiagaraFunctionLibrary.h"

UBaseSkill::UBaseSkill()
{
    // 기본값 설정
    SkillID = NAME_None;
    SkillName = TEXT("Unknown Skill");
    Description = TEXT("No description available.");
    SkillType = ESkillType::Melee;
    CooldownTime = 1.0f;
    SkillDamage = 10.0f;
    RemainingCooldown = 0.0f;
    bIsActive = false;
    SkillTimer = 0.0f;
    
    // 기본 공격 속도 및 데미지 추가
    DefaultAttackSpeed = 1.0f;
    DefaultBaseDamage = 0.0f;
    
    // 기본 효과 파라미터 설정
    EffectParams.LaunchDirectionMultiplier = FVector(500.0f, 0.0f, 100.0f);
    EffectParams.bVerticalLaunch = false;
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
    // 쿨다운 체크
    if (RemainingCooldown > 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("스킬 %s 활성화 실패: 쿨다운 중 (남은 시간: %f)"), 
            *SkillName, RemainingCooldown);
        return false;
    }
    
    // 소유자 캐릭터 확인
    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("스킬 %s 활성화 실패: 소유자 캐릭터 없음"), *SkillName);
        return false;
    }
    
    // 이미 활성화 상태라면 중복 활성화 방지
    if (bIsActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("스킬 %s 활성화 실패: 이미 활성화 상태"), *SkillName);
        return false;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("스킬 %s 활성화 시작"), *SkillName);
    
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
   // 자식 클래스에서 구현 가능
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

/**
 * 스킬 효과 파라미터 가져오기 (자식 클래스에서 오버라이드 가능)
 */
FSkillEffectParams UBaseSkill::GetEffectParams()
{
    return EffectParams;
}

/**
 * 최종 데미지 계산 로직
 */
float UBaseSkill::CalculateDamage(float AdditionalDamage)
{
    float FinalDamage = SkillDamage + AdditionalDamage;
    
    // 아이템 데미지 추가
    if (ItemData)
    {
        FinalDamage += ItemData->BaseDamage;
    }
    else if (DefaultBaseDamage > 0.0f)
    {
        FinalDamage += DefaultBaseDamage;
    }
    
    return FinalDamage;
}

/**
 * 이펙트 생성 유틸리티 함수
 */
void UBaseSkill::SpawnEffect(const FVector& Location, const FRotator& Rotation)
{
    if (!SkillEffect.IsNull() && OwnerCharacter)
    {
        UNiagaraSystem* Effect = SkillEffect.LoadSynchronous();
        if (Effect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                OwnerCharacter->GetWorld(),
                Effect,
                Location,
                Rotation
            );
        }
    }
}

/**
 * 단일 대상에 효과 적용
 */
bool UBaseSkill::ApplyEffectToTarget(AActor* Target, float Damage)
{
    if (!Target || Target == OwnerCharacter || !OwnerCharacter)
    {
        return false;
    }
    
    // 데미지 및 효과 파라미터 얻기
    float FinalDamage = CalculateDamage(Damage);
    FSkillEffectParams Params = GetEffectParams();
    
    // 넉백/런치 방향 계산
    FVector LaunchDirection = OwnerCharacter->GetActorForwardVector();
    FVector LaunchVector;
    
    if (Params.bVerticalLaunch)
    {
        // 수직 런치
        LaunchVector = FVector(
            LaunchDirection.X * Params.LaunchDirectionMultiplier.X * 0.1f,
            0.0f,
            Params.LaunchDirectionMultiplier.Z
        );
    }
    else
    {
        // 일반 런치/넉백
        LaunchVector = FVector(
            LaunchDirection.X * Params.LaunchDirectionMultiplier.X,
            LaunchDirection.Y * Params.LaunchDirectionMultiplier.Y,
            Params.LaunchDirectionMultiplier.Z
        );
    }
    
    // IDamageSystemUser 인터페이스를 구현한 타겟인지 확인
    if (auto CastedTarget = Cast<IDamageSystemUser>(Target))
    {
        // 데미지 구조체 생성
        FAttackData AttackData
        {
            OwnerCharacter,
            FinalDamage,
            LaunchVector,
            Params.Effects,
            Params.Durations,
            {},
            {}
        };
        
        // 데미지 및 효과 적용
        CastedTarget->OnAttacked(AttackData);
        
        // 이펙트 생성
        SpawnEffect(Target->GetActorLocation(), OwnerCharacter->GetActorRotation());
        
        return true;
    }
    
    return false;
}

void UBaseSkill::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
}

/**
 * 공통 ProcessTargetActors 구현
 * 모든 자식 클래스에서 공유할 수 있는 코드
 */
int32 UBaseSkill::ProcessTargetActors(const TArray<AActor*>& TargetActors, float AdditionalDamage)
{
    // 기본 유효성 검사
    if (!OwnerCharacter || !OwnerCharacter->HasAuthority())
    {
        UE_LOG(LogTemp, Error, TEXT("[UBaseSkill::ProcessTargetActors] 유효하지 않은 소유자 캐릭터 또는 권한 없음"));
        return 0;
    }
    
    int32 ProcessedCount = 0;
    
    // 모든 타겟에 효과 적용
    for (AActor* Target : TargetActors)
    {
        if (!Target || Target == OwnerCharacter)
        {
            continue;
        }
        
        // 단일 타겟 효과 적용
        if (ApplyEffectToTarget(Target, AdditionalDamage))
        {
            ProcessedCount++;
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("[UBaseSkill::ProcessTargetActors] 처리된 타겟 수: %d"), ProcessedCount);
    return ProcessedCount;
}