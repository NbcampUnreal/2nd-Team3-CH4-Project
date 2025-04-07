#include "SkillComponent.h"
#include "WeaponMaster/Characters/TestCharacter.h"
#include "WeaponMaster/Data/ItemDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "TimerManager.h"
#include "WeaponMaster/Skills/BaseSkill.h"

USkillComponent::USkillComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
    bWantsInitializeComponent = true;
}

void USkillComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // 소유자 캐릭터 캐싱
    OwnerCharacter = Cast<ATestCharacter>(GetOwner());
    
    // 쿨다운 업데이트 타이머 설정 (0.1초마다)
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (GetOwner()->HasAuthority() || (OwnerPawn && OwnerPawn->IsLocallyControlled()))
    {
        GetWorld()->GetTimerManager().SetTimer(
            CooldownUpdateTimerHandle,
            this,
            &USkillComponent::UpdateSkillCooldowns,
            0.1f,
            true
        );
    }
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // 필요한 틱 로직 (대부분의 업데이트는 타이머로 처리)
}

void USkillComponent::InitializeSkillsFromItem(UItemDataAsset* NewItem)
{
    // 클라이언트에서 호출되면 서버로 요청
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_InitializeSkills(NewItem);
        return;
    }
    
    // 기존 스킬 정리
    for (UBaseSkill* Skill : Skills)
    {
        if (Skill)
        {
            Skill->ConditionalBeginDestroy();
        }
    }
    Skills.Empty();
    
    // 아이템이 없으면 스킬 초기화만 하고 리턴
    if (!NewItem)
    {
        OnSkillsUpdated.Broadcast(Skills);
        Client_UpdateSkills(Skills);
        return;
    }
    
    TArray<TSubclassOf<UBaseSkill>> SkillClasses = NewItem->SkillClasses;
    
    // 스킬 생성 및 초기화
    for (TSubclassOf<UBaseSkill> SkillClass : SkillClasses)
    {
        if (SkillClass)
        {
            UBaseSkill* NewSkill = CreateSkill(SkillClass, NewItem);
            if (NewSkill)
            {
                Skills.Add(NewSkill);
            }
        }
    }
    
    // 스킬 업데이트 이벤트 발생
    OnSkillsUpdated.Broadcast(Skills);
    
    // 클라이언트에게 전송
    Client_UpdateSkills(Skills);
}

UBaseSkill* USkillComponent::CreateSkill(TSubclassOf<UBaseSkill> SkillClass, UItemDataAsset* OwnerItem)
{
    if (!SkillClass || !OwnerCharacter)
    {
        return nullptr;
    }
    
    // 스킬 객체 생성
    UBaseSkill* NewSkill = NewObject<UBaseSkill>(this, SkillClass);
    if (NewSkill)
    {
        // 스킬 초기화
        NewSkill->Initialize(OwnerCharacter, OwnerItem);
    }
    
    return NewSkill;
}

bool USkillComponent::ExecuteSkill(int32 SkillIndex)
{
    // 클라이언트에서 호출되면 서버로 전달
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_ExecuteSkill(SkillIndex);
        return true; // 클라이언트는 서버의 응답을 기다림
    }
    
    // 스킬 인덱스 유효성 검사
    if (!Skills.IsValidIndex(SkillIndex) || !Skills[SkillIndex])
    {
        return false;
    }
    
    // 스킬 사용 시도
    return Skills[SkillIndex]->ActivateSkill();
}

/**
 * 주어진 스킬 ID에 해당하는 스킬을 실행합니다.
 * 스킬 ID를 기반으로 인덱스를 검색하고, 해당 인덱스를 사용하여 스킬을 실행합니다.
 *
 * @param SkillID 실행할 스킬의 ID
 * @return 스킬 실행 성공 여부 (스킬이 없거나 실행 실패 시 false 반환)
 */
//사용하진 않음
bool USkillComponent::ExecuteSkillByID(FName SkillID)
{
    int32 SkillIndex = GetSkillIndex(SkillID);
    if (SkillIndex != -1)
    {
        return ExecuteSkill(SkillIndex);
    }
    return false;
}

/**
 * 주어진 인덱스에 해당하는 스킬 객체를 반환합니다.
 * 스킬 배열에서 유효한 인덱스를 확인한 후 해당 인덱스의 스킬을 검색합니다.
 *
 * @param SkillIndex 검색할 스킬의 인덱스
 * @return 찾은 스킬 객체 (유효하지 않은 인덱스인 경우 nullptr 반환)
 */
UBaseSkill* USkillComponent::GetSkill(int32 SkillIndex) const
{
    if (Skills.IsValidIndex(SkillIndex))
    {
        return Skills[SkillIndex];
    }
    return nullptr;
}

/**
 * 주어진 스킬 ID에 해당하는 스킬 객체를 반환합니다.
 * 스킬 목록에서 해당 ID와 일치하는 스킬을 검색합니다.
 *
 * @param SkillID 검색할 스킬의 ID
 * @return 찾은 스킬 객체 (스킬이 없으면 nullptr 반환)
 */
UBaseSkill* USkillComponent::GetSkillByID(FName SkillID) const
{
    for (UBaseSkill* Skill : Skills)
    {
        if (Skill && Skill->GetSkillID() == SkillID)
        {
            return Skill;
        }
    }
    return nullptr;
}

/**
 * 주어진 스킬 ID에 해당하는 스킬의 인덱스를 반환합니다.
 * 스킬 목록에서 해당 ID와 일치하는 스킬을 검색합니다.
 *
 * @param SkillID 검색할 스킬의 ID
 * @return 찾은 스킬의 인덱스 (스킬이 없으면 -1 반환)
 */
int32 USkillComponent::GetSkillIndex(FName SkillID) const
{
    for (int32 i = 0; i < Skills.Num(); i++)
    {
        if (Skills[i] && Skills[i]->GetSkillID() == SkillID)
        {
            return i;
        }
    }
    return -1;
}

void USkillComponent::ResetSkillCooldown(int32 SkillIndex)
{
    // 스킬 인덱스 유효성 검사
    if (Skills.IsValidIndex(SkillIndex) && Skills[SkillIndex])
    {
        // 쿨다운 초기화
        UBaseSkill* Skill = Skills[SkillIndex];
        Skill->UpdateCooldown(Skill->GetCooldownTime());
    }
}

// 현재 사용되진 않음
void USkillComponent::ResetAllSkillCooldowns()
{
    // 모든 스킬의 쿨다운 초기화
    for (int32 i = 0; i < Skills.Num(); i++)
    {
        ResetSkillCooldown(i);
    }
}

// 사용되진 않은데 위젯에서 쓸듯?
float USkillComponent::GetSkillCooldownProgress(int32 SkillIndex) const
{
    // 스킬 인덱스 유효성 검사
    if (Skills.IsValidIndex(SkillIndex) && Skills[SkillIndex])
    {
        return Skills[SkillIndex]->GetCooldownProgress();
    }
    return 1.0f; // 유효하지 않은 인덱스면 쿨다운 완료(1.0) 반환
}

//이것도 위젯에서 쓸수도 있음
float USkillComponent::GetSkillRemainingCooldown(int32 SkillIndex) const
{
    // 스킬 인덱스 유효성 검사
    if (Skills.IsValidIndex(SkillIndex) && Skills[SkillIndex])
    {
        return Skills[SkillIndex]->GetRemainingCooldown();
    }
    return 0.0f; // 유효하지 않은 인덱스면 쿨다운 없음(0.0) 반환
}

// 쿨타운을 타이머로 계속 업데이트
void USkillComponent::UpdateSkillCooldowns()
{
    // 모든 스킬의 쿨다운 업데이트
    float DeltaTime = 0.1f; // 타이머 간격과 동일하게 설정
    
    for (UBaseSkill* Skill : Skills)
    {
        if (Skill)
        {
            Skill->UpdateCooldown(DeltaTime);
        }
    }
}

void USkillComponent::Server_InitializeSkills_Implementation(UItemDataAsset* NewItem)
{
    // 서버에서 스킬 초기화 진행
    InitializeSkillsFromItem(NewItem);
}

void USkillComponent::Server_ExecuteSkill_Implementation(int32 SkillIndex)
{
    // 서버에서 스킬 실행
    ExecuteSkill(SkillIndex);
}

void USkillComponent::Client_UpdateSkills_Implementation(const TArray<UBaseSkill*>& UpdatedSkills)
{
    // 클라이언트에서 스킬 업데이트
    Skills = UpdatedSkills;
    OnSkillsUpdated.Broadcast(Skills);
}

void USkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // 스킬 목록 복제
    DOREPLIFETIME(USkillComponent, Skills);
}