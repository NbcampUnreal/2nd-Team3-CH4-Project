#include "SkillComponent.h"
#include "WeaponMaster/Characters/TestCharacter.h"
#include "WeaponMaster/Data/ItemDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "TimerManager.h"
#include "WeaponMaster/Characters/Components/ItemComponent/ItemComponent.h"
#include "WeaponMaster/Data/SkillDataAsset.h"
#include "WeaponMaster/Skills/BaseSkill.h"

USkillComponent::USkillComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
    bWantsInitializeComponent = true;

    OpportunityWindowDuration = 4.0f;
}

void USkillComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // 소유자 캐릭터 캐싱
    OwnerCharacter = Cast<ACharacter>(GetOwner());
    
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
    
    // 마지막으로 사용된 스킬 인덱스 배열 초기화
    LastUsedSkillIndices.Empty();
    // 그룹의 리셋 준비 상태 배열 초기화
    GroupReadyToReset.Empty();
    // 그룹 스킬 오프셋 배열 초기화
    GroupSkillOffsets.Empty();
    // 캐싱된 스킬 그룹 배열 초기화
    CachedSkillGroups.Empty();

    //기회시간 초기화
    OpportunityTimers.Empty();
    IsInOpportunityWindow.Empty();
    // 아이템 데이터 캐싱
    CachedItemData = NewItem;
    
    // 아이템이 없으면 스킬 초기화만 하고 리턴
    if (!NewItem)
    {
        OnSkillsUpdated.Broadcast(Skills);
        Client_UpdateSkills(Skills);
        return;
    }
    
    // 아이템의 스킬 그룹 캐싱
    CachedSkillGroups = NewItem->SkillGroups;
    
    // 각 스킬 그룹 처리
    int32 CurrentSkillOffset = 0;
    
    for (int32 GroupIndex = 0; GroupIndex < CachedSkillGroups.Num(); ++GroupIndex)
    {
        const FSkillGroupArray& SkillGroup = CachedSkillGroups[GroupIndex];
        
        // 그룹의 시작 인덱스 저장
        GroupSkillOffsets.Add(CurrentSkillOffset);
        
        // 상태 배열 초기화
        LastUsedSkillIndices.Add(0);           // 각 그룹은 0번 인덱스부터 시작
        GroupReadyToReset.Add(true);           // 초기 상태는 리셋 준비 완료
        OpportunityTimers.Add(0.0f);           // 기회 시간 타이머 초기화
        IsInOpportunityWindow.Add(false);      // 기본적으로 기회 시간 모드 비활성화
        
        // 그룹 내 각 스킬 클래스 처리
        for (TSubclassOf<UBaseSkill> SkillClass : SkillGroup.SkillClasses)
        {
            if (SkillClass)
            {
                UBaseSkill* NewSkill = CreateSkill(SkillClass, NewItem);
                if (NewSkill)
                {
                    Skills.Add(NewSkill);
                    CurrentSkillOffset++;
                }
            }
        }
    }
    
    // 스킬 업데이트 이벤트 발생
    OnSkillsUpdated.Broadcast(Skills);
    
    // 클라이언트에게 전송
    Client_UpdateSkills(Skills);
}

UBaseSkill* USkillComponent::GetActiveSkill() const
{
    // 모든 스킬을 순회하며 활성화된 스킬 찾기
    for (UBaseSkill* Skill : Skills)
    {
        if (Skill && Skill->IsSkillActive())
        {
            return Skill;
        }
    }
    
    // 활성화된 스킬이 없으면 nullptr 반환
    return nullptr;
}

void USkillComponent::OnRep_SkillData()
{
    // 복제된 데이터를 받으면 로컬 스킬 객체 업데이트
    UpdateLocalSkillsFromReplicatedData();
    
    // UI 등에 스킬 업데이트 알림
    OnSkillsUpdated.Broadcast(Skills);
}

void USkillComponent::UpdateReplicatedData()
{
    // 서버에서만 실행
    if (!GetOwner()->HasAuthority())
        return;
        
    // 복제 데이터 배열 초기화
    ReplicatedSkillData.Empty(Skills.Num());
    
    // 각 스킬에서 필요한 데이터만 추출하여 복제 배열에 추가
    for (UBaseSkill* Skill : Skills)
    {
        if (!Skill) continue;
        
        FReplicatedSkillData SkillData;
        SkillData.SkillClass = Skill->GetClass();
        SkillData.SkillID = Skill->GetSkillID();
        SkillData.RemainingCooldown = Skill->GetRemainingCooldown();
        SkillData.bIsActive = Skill->IsSkillActive();
        
        // 필요시 추가 커스텀 데이터 저장
        // ...
        
        ReplicatedSkillData.Add(SkillData);
    }
}

void USkillComponent::UpdateLocalSkillsFromReplicatedData()
{
    // 클라이언트에서만 의미 있음
    if (GetOwner()->HasAuthority())
        return;
        
    // 기존 스킬 객체와 새 데이터 매칭
    // 새 스킬이 필요하면 생성, 기존 스킬은 업데이트
    
    TArray<UBaseSkill*> NewSkills;
    
    for (const FReplicatedSkillData& SkillData : ReplicatedSkillData)
    {
        // 기존 스킬 찾기
        UBaseSkill* ExistingSkill = nullptr;
        for (UBaseSkill* Skill : Skills)
        {
            if (Skill && Skill->GetSkillID() == SkillData.SkillID)
            {
                ExistingSkill = Skill;
                break;
            }
        }
        
        // 스킬이 없으면 새로 생성
        if (!ExistingSkill)
        {
            if (SkillData.SkillClass)
            {
                ExistingSkill = NewObject<UBaseSkill>(this, SkillData.SkillClass);
                if (ExistingSkill)
                {
                    // 스킬 초기화
                    ExistingSkill->Initialize(Cast<ACharacter>(GetOwner()), nullptr);
                }
            }
        }
        
        // 스킬이 있으면 데이터 업데이트
        if (ExistingSkill)
        {
            // 여기서 스킬 상태 업데이트 (복제된 데이터 적용)
            // 직접 멤버 변수를 수정하는 대신 아래와 같은 헬퍼 함수 추가하는 것이 좋음
            ExistingSkill->UpdateFromReplicatedData(SkillData.RemainingCooldown, SkillData.bIsActive);
            
            NewSkills.Add(ExistingSkill);
        }
    }
    
    // 로컬 스킬 목록 업데이트
    Skills = NewSkills;
}

void USkillComponent::HandleSkillStarted(UBaseSkill* Skill)
{
    if (Skill)
    {
        // 스킬 시작 이벤트 브로드캐스트
        OnSkillStarted.Broadcast(Skill);
        UE_LOG(LogTemp, Log, TEXT("[USkillComponent::HandleSkillStarted] 스킬 시작: %s"), *Skill->GetSkillName());
    }
}

void USkillComponent::HandleSkillEnded(UBaseSkill* Skill)
{
    if (Skill)
    {
        // 스킬 종료 이벤트 브로드캐스트
        OnSkillEnded.Broadcast(Skill);
        UE_LOG(LogTemp, Log, TEXT("[USkillComponent::HandleSkillEnded] 스킬 종료: %s"), *Skill->GetSkillName());
    }
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
        
        // 스킬 이벤트 바인딩
        NewSkill->OnSkillStarted.AddDynamic(this, &USkillComponent::HandleSkillStarted);
        NewSkill->OnSkillEnded.AddDynamic(this, &USkillComponent::HandleSkillEnded);
    }
    
    return NewSkill;
}



bool USkillComponent::ExecuteSkill(int32 GroupIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("[%s] 스킬 그룹 %d 실행 시도 (권한: %d)"), 
    GetOwnerRole() == ROLE_Authority ? TEXT("서버") : TEXT("클라이언트"),
    GroupIndex,
    (int32)GetOwnerRole());
    // 클라이언트에서 호출되면 서버로 전달
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_ExecuteSkill(GroupIndex);
        return true; // 클라이언트는 서버의 응답을 기다림
    }
    
    // 그룹 인덱스 유효성 검사
    if (!CachedSkillGroups.IsValidIndex(GroupIndex) || !GroupSkillOffsets.IsValidIndex(GroupIndex))
    {
        return false;
    }
    
    const FSkillGroupArray& SkillGroup = CachedSkillGroups[GroupIndex];
    
    // 스킬 그룹이 비어있으면 실패
    if (SkillGroup.SkillClasses.Num() == 0)
    {
        return false;
    }
    
    int32 CurrentIndex = LastUsedSkillIndices[GroupIndex];
    int32 NextIndex = CurrentIndex;
    
    // 현재 기회 시간 모드인지 확인
    if (IsInOpportunityWindow[GroupIndex])
    {
        // 기회 시간 모드에서는 다음 스킬로 이동
        NextIndex = (CurrentIndex + 1) % SkillGroup.SkillClasses.Num();
    }
    else
    {
        // 기회 시간 모드가 아니면 항상 0번 스킬 사용
        NextIndex = 0;
    }
    
    // 그룹 내 시작 오프셋 가져오기
    int32 SkillOffset = GroupSkillOffsets[GroupIndex];
    
    // 실제 Skills 배열에서의 인덱스
    int32 ActualSkillIndex = SkillOffset + NextIndex;
    
    // 스킬 인덱스 유효성 검사
    if (!Skills.IsValidIndex(ActualSkillIndex) || !Skills[ActualSkillIndex])
    {
        return false;
    }
    
    // 스킬 사용 시도
    bool Success = Skills[ActualSkillIndex]->ActivateSkill();

    UE_LOG(LogTemp, Warning, TEXT("스킬 실행 %s (그룹: %d, 인덱스: %d, 스킬: %s)"), 
        Success ? TEXT("성공") : TEXT("실패"),
        GroupIndex, NextIndex, 
        *Skills[ActualSkillIndex]->GetSkillName());
    
    // 스킬 실행 성공 시 상태 업데이트
    if (Success)
    {
        // 현재 인덱스 업데이트
        LastUsedSkillIndices[GroupIndex] = NextIndex;
        
        // 다음 스킬이 있는지 확인 (마지막 스킬이 아닌 경우)
        bool HasNextSkill = (NextIndex + 1) < SkillGroup.SkillClasses.Num();
        
        if (HasNextSkill)
        {
            // 기회 시간 시작
            IsInOpportunityWindow[GroupIndex] = true;
            OpportunityTimers[GroupIndex] = OpportunityWindowDuration;
            UE_LOG(LogTemp, Log, TEXT("그룹 %d의 기회 시간 시작 (%f초), 다음 스킬 인덱스: %d"), 
                GroupIndex, OpportunityWindowDuration, NextIndex + 1);
        }
        else
        {
            // 마지막 스킬이면 기회 시간 종료 및 리셋
            IsInOpportunityWindow[GroupIndex] = false;
            LastUsedSkillIndices[GroupIndex] = 0;
            GroupReadyToReset[GroupIndex] = true;
            UE_LOG(LogTemp, Log, TEXT("그룹 %d의 마지막 스킬 사용됨, 다음 사용 시 첫 번째 스킬부터 시작합니다."), GroupIndex);
        }

        UpdateReplicatedData();
    }
    
    return Success;
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
    float DeltaTime = 0.1f;  // 타이머 간격
    
    // 모든 스킬 쿨다운 업데이트
    for (UBaseSkill* Skill : Skills)
    {
        if (Skill)
        {
            Skill->UpdateCooldown(DeltaTime);
        }
    }
    
    // 기회 시간 타이머 업데이트
    for (int32 GroupIndex = 0; GroupIndex < OpportunityTimers.Num(); ++GroupIndex)
    {
        // 이 그룹이 기회 시간 모드인 경우
        if (IsInOpportunityWindow[GroupIndex])
        {
            // 타이머 감소
            OpportunityTimers[GroupIndex] = FMath::Max(0.0f, OpportunityTimers[GroupIndex] - DeltaTime);
            
            // 타이머가 끝났으면 기회 시간 종료
            if (OpportunityTimers[GroupIndex] <= 0.0f)
            {
                IsInOpportunityWindow[GroupIndex] = false;
                LastUsedSkillIndices[GroupIndex] = 0; // 0번 스킬로 리셋
                GroupReadyToReset[GroupIndex] = true;
                
                UE_LOG(LogTemp, Log, TEXT("그룹 %d의 기회 시간 종료, 다음 사용 시 첫 번째 스킬부터 시작합니다."), GroupIndex);
            }
        }
    }

    if (GetOwner()->HasAuthority())
    {
        UpdateReplicatedData();
    }
}

int32 USkillComponent::GetNextSkillIndexInGroup(int32 GroupIndex)
{
    // 그룹 인덱스 유효성 검사
    if (!LastUsedSkillIndices.IsValidIndex(GroupIndex) || !CachedSkillGroups.IsValidIndex(GroupIndex))
    {
        return 0;
    }
    
    const FSkillGroupArray& SkillGroup = CachedSkillGroups[GroupIndex];
    
    // 그룹이 비어있으면 0 반환
    if (SkillGroup.SkillClasses.Num() == 0)
    {
        return 0;
    }
    
    int32 CurrentIndex = LastUsedSkillIndices[GroupIndex];
    
    // 그룹의 시작 인덱스 가져오기
    int32 SkillOffset = GroupSkillOffsets[GroupIndex];
    
    // 실제 Skills 배열에서의 인덱스
    int32 ActualSkillIndex = SkillOffset + CurrentIndex;
    
    // 해당 스킬이 유효하고 쿨다운이 완료되었는지 확인
    if (Skills.IsValidIndex(ActualSkillIndex) && Skills[ActualSkillIndex] && Skills[ActualSkillIndex]->IsSkillReady())
    {
        // 쿨다운이 완료되었다면 첫 번째 스킬로 리셋
        if (GroupReadyToReset[GroupIndex])
        {
            LastUsedSkillIndices[GroupIndex] = 0;
            return 0;
        }
    }
    
    // 다음 스킬 인덱스 계산
    int32 NextIndex = (CurrentIndex + 1) % SkillGroup.SkillClasses.Num();
    
    // 그룹의 마지막 스킬이면 리셋 플래그 설정
    if (NextIndex == 0)
    {
        GroupReadyToReset[GroupIndex] = true;
    }
    else
    {
        GroupReadyToReset[GroupIndex] = false;
    }
    
    // 다음 인덱스 저장 및 반환
    LastUsedSkillIndices[GroupIndex] = NextIndex;
    return NextIndex;
}

void USkillComponent::Server_InitializeSkills_Implementation(UItemDataAsset* NewItem)
{
    // 서버에서 스킬 초기화 진행
    InitializeSkillsFromItem(NewItem);
}

void USkillComponent::Server_ExecuteSkill_Implementation(int32 GroupIndex)
{
    // 서버에서 스킬 실행
    ExecuteSkill(GroupIndex);
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
    DOREPLIFETIME(USkillComponent, ReplicatedSkillData);
}

void USkillComponent::InitializeSkills(USkillDataAsset* NewSkill)
{
    // 클라이언트에서 호출되면 서버로 요청
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_InitializeSkillsFromDataAsset(NewSkill);
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
    
    // 마지막으로 사용된 스킬 인덱스 배열 초기화
    LastUsedSkillIndices.Empty();
    // 그룹의 리셋 준비 상태 배열 초기화
    GroupReadyToReset.Empty();
    // 그룹 스킬 오프셋 배열 초기화
    GroupSkillOffsets.Empty();
    // 캐싱된 스킬 그룹 배열 초기화
    CachedSkillGroups.Empty();

    // 기회시간 초기화
    OpportunityTimers.Empty();
    IsInOpportunityWindow.Empty();
    
    // 아이템 데이터는 null로 설정 (스킬만 사용하므로)
    CachedItemData = nullptr;
    
    // 스킬 데이터가 없으면 스킬 초기화만 하고 리턴
    if (!NewSkill)
    {
        OnSkillsUpdated.Broadcast(Skills);
        Client_UpdateSkills(Skills);
        return;
    }
    
    // 스킬의 그룹 캐싱
    CachedSkillGroups = NewSkill->SkillGroups;
    
    // 각 스킬 그룹 처리
    int32 CurrentSkillOffset = 0;
    
    for (int32 GroupIndex = 0; GroupIndex < CachedSkillGroups.Num(); ++GroupIndex)
    {
        const FSkillGroupArray& SkillGroup = CachedSkillGroups[GroupIndex];
        
        // 그룹의 시작 인덱스 저장
        GroupSkillOffsets.Add(CurrentSkillOffset);
        
        // 상태 배열 초기화
        LastUsedSkillIndices.Add(0);           // 각 그룹은 0번 인덱스부터 시작
        GroupReadyToReset.Add(true);           // 초기 상태는 리셋 준비 완료
        OpportunityTimers.Add(0.0f);           // 기회 시간 타이머 초기화
        IsInOpportunityWindow.Add(false);      // 기본적으로 기회 시간 모드 비활성화
        
        // 그룹 내 각 스킬 클래스 처리
        for (TSubclassOf<UBaseSkill> SkillClass : SkillGroup.SkillClasses)
        {
            if (SkillClass)
            {
                UBaseSkill* NewSkillObj = CreateSkill(SkillClass, nullptr);
                if (NewSkillObj)
                {
                    Skills.Add(NewSkillObj);
                    CurrentSkillOffset++;
                }
            }
        }
    }
    
    // 스킬 업데이트 이벤트 발생
    OnSkillsUpdated.Broadcast(Skills);
    
    // 클라이언트에게 전송
    Client_UpdateSkills(Skills);
}

// 스킬 데이터 애셋으로 초기화하는 서버 RPC
void USkillComponent::Server_InitializeSkillsFromDataAsset_Implementation(USkillDataAsset* NewSkill)
{
    InitializeSkills(NewSkill);
}