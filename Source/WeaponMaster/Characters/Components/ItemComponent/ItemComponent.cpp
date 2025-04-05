// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemComponent.h"

#include "WeaponMaster/Characters/TestCharacter.h"
#include "WeaponMaster/Characters/WeaponMasterCharacter.h"
#include "WeaponMaster/Data/GameDataManager.h"
#include "WeaponMaster/Data/ItemDataAsset.h"
#include "WeaponMaster/Data/WeaponMasterLib.h"
#include "WeaponMaster/Skills/SkillManager.h"


UItemComponent::UItemComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    bWantsInitializeComponent = true;
    
    // 네트워크 복제 설정
    SetIsReplicatedByDefault(true);
    
    // 기본값 초기화
    EquippedItem = nullptr;
}

void UItemComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // 스킬 매니저 생성
    SkillManager = NewObject<USkillManager>(this);
    
    // 소유자 캐릭터 가져오기
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        // 초기 설정
    }
}

void UItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // 필요한 틱 로직
}


bool UItemComponent::EquipItem(FName ItemID)
{
    // 클라이언트에서 호출되었으면 서버에 요청
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_EquipItem(ItemID);
        return true; // 클라이언트는 서버 응답을 기다림
    }
    
    // 현재 아이템과 동일하면 무시
    if (EquippedItem && EquippedItem->ItemID == ItemID)
    {
        return true;
    }
    
    // 이전 아이템 장착 해제
    if (EquippedItem)
    {
        UnequipItem();
    }
    
    // GameDataManager에서 아이템 정보 가져오기
    UGameDataManager* GameDataManager = UWeaponMasterLib::GetGameDataManager(GetWorld());
    if (!GameDataManager)
    {
        return false;
    }
    
    UItemDataAsset* NewItem = GameDataManager->GetItemData(ItemID);
    if (!NewItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("Item with ID %s not found"), *ItemID.ToString());
        return false;
    }
    
    // 아이템 메시 비동기 로드
    if (!NewItem->EquippedMesh.IsNull())
    {
        // 비동기 로드 시작
        UAssetManager& AssetManager = UAssetManager::Get();
        FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
        
        LoadHandle = StreamableManager.RequestAsyncLoad(
            NewItem->EquippedMesh.ToSoftObjectPath(),
            FStreamableDelegate::CreateUObject(this, &UItemComponent::OnMeshLoadCompleted)
        );
        
        // 로드 중 임시 상태 설정
        EquippedItem = NewItem;
        return true;
    }
    else
    {
        // 메시가 없는 경우 바로 장착 완료
        EquippedItem = NewItem;
        AttachItemToSocket(NewItem);
        UpdateAvailableSkills();
        OnItemEquipped.Broadcast(NewItem);
        
        // 클라이언트에게 알림
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter && OwnerCharacter->GetLocalRole() == ROLE_Authority)
        {
            Client_OnItemEquipped(NewItem);
        }
        
        return true;
    }
}

/**
 * 현재 장착된 아이템을 해제합니다.
 *
 * 장착 해제 시 다음과 같은 작업이 수행됩니다:
 * - 아이템의 시각 효과 제거
 * - 무기 충돌 비활성화
 * - 스킬 목록 초기화
 * - 장착 해제 이벤트 발생
 * - 장착된 아이템 참조 해제
 */
void UItemComponent::UnequipItem()
{
    if (!EquippedItem)
    {
        return;
    }
    // 장착 해제한 아이템 드롭 로직 추가해야함
    
    // 아이템 시각 효과 처리
    HandleItemVisualEffects(EquippedItem, false);
    
    // 무기 충돌 비활성화
    SetWeaponCollisionEnabled(false);
    
    // 스킬 세트 초기화
    AvailableSkills.Empty();
    
    // 장착 해제 이벤트 발생
    OnItemUnequipped.Broadcast();
    
    // 아이템 참조 해제
    EquippedItem = nullptr;
}

/**
 * 현재 장착된 아이템을 반환합니다.
 *
 * @return 현재 장착된 아이템 데이터 자산(UItemDataAsset)입니다. 장착된 아이템이 없을 경우 nullptr를 반환합니다.
 */
UItemDataAsset* UItemComponent::GetEquippedItem() const
{
    return EquippedItem;
}

/**
 * 스킬을 실행합니다 (인덱스 기반).
 *
 * @param SkillIndex 실행할 스킬의 인덱스
 */
void UItemComponent::ExecuteSkill(int32 SkillIndex)
{
    if (!EquippedItem || !SkillManager)
    {
        return;
    }
    
    // 소유자 확인
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        return;
    }
    
    // 인덱스 범위 검사
    if (SkillIndex < 0 || SkillIndex >= AvailableSkills.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skill index: %d"), SkillIndex);
        return;
    }
    
    // 인덱스 기반 스킬 실행
    SkillManager->ExecuteSkill(SkillIndex, OwnerCharacter);
}

/**
 * 지정된 ItemID를 사용하여 아이템 데이터를 비동기로 로드합니다.
 *
 * @param ItemID 로드할 아이템의 고유 식별자 이름입니다.
 */
void UItemComponent::AsyncLoadItem(FName ItemID)
{
    // GameDataManager에서 아이템 정보 가져오기
    UGameDataManager* GameDataManager = UWeaponMasterLib::GetGameDataManager(this);
    if (!GameDataManager)
    {
        return;
    }
    
    UItemDataAsset* ItemData = GameDataManager->GetItemData(ItemID);
    if (!ItemData || ItemData->EquippedMesh.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to start async load for item %s"), *ItemID.ToString());
        return;
    }
    
    // 비동기 로드 시작
    UAssetManager& AssetManager = UAssetManager::Get();
    FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
    
    TArray<FSoftObjectPath> ItemsToLoad;
    ItemsToLoad.Add(ItemData->EquippedMesh.ToSoftObjectPath());
    
    LoadHandle = StreamableManager.RequestAsyncLoad(
        ItemsToLoad,
        FStreamableDelegate::CreateUObject(this, &UItemComponent::OnItemLoaded, ItemID)
    );
}

/**
 * 비동기 로드 완료 후, 아이템 데이터를 처리하고 소켓에 아이템을 부착합니다.
 *
 * @param ItemID 처리해야 할 아이템의 고유 식별자
 */
void UItemComponent::OnItemLoaded(FName ItemID)
{
    // 로드 핸들 정리
    LoadHandle.Reset();

    UGameDataManager* GameDataManager = UWeaponMasterLib::GetGameDataManager(this);
    if (!GameDataManager)
    {
        return;
    }
    
    // 아이템 장착 처리
    UItemDataAsset* ItemData = GameDataManager->GetItemData(ItemID);
    if (!ItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("Item data not found after async load: %s"), *ItemID.ToString());
        return;
    }
    
    // 아이템 소켓에 부착
    AttachItemToSocket(ItemData);
}

/**
 * 메시 로드 완료 시 호출되는 콜백 함수입니다.
 *
 * 이 메서드는 아이템의 메시 로드가 완료된 후 수행되는 처리를 다룹니다:
 * - 장착된 아이템을 캐릭터 소켓에 부착
 * - 사용 가능한 스킬 목록 업데이트
 * - 아이템 장착 완료 이벤트 발생
 * - 비동기 로드 핸들 초기화
 */
void UItemComponent::OnMeshLoadCompleted()
{
    // 로드 완료 후 처리
    if (EquippedItem)
    {
        // 아이템 소켓에 부착
        AttachItemToSocket(EquippedItem);
        
        // 사용 가능한 스킬 업데이트
        UpdateAvailableSkills();
        
        // 이벤트 발생
        OnItemEquipped.Broadcast(EquippedItem);
    }
    
    // 로드 핸들 정리
    LoadHandle.Reset();
}

/**
 * 아이템을 지정된 소켓에 부착합니다.
 *
 * 이 메서드는 아이템 메시를 로드하여 캐릭터의 소켓에 부착하고,
 * 시각 효과와 충돌 처리를 설정합니다. 기본적으로 충돌은 비활성화된
 * 상태로 시작됩니다.
 *
 * @param Item 장착할 아이템 데이터. 유효하지 않은 경우 아무 동작도 수행하지 않습니다.
 */
void UItemComponent::AttachItemToSocket(UItemDataAsset* Item)
{
    if (!Item)
    {
        return;
    }
    
    // 소유자 캐릭터 가져오기
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        return;
    }
    
    // 메시 컴포넌트에 접근
    USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh();
    if (!MeshComp)
    {
        return;
    }
    
    // 메시 로드 확인
    UStaticMesh* ItemMesh = Item->EquippedMesh.Get();
    if (!ItemMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load mesh for item %s"), *Item->ItemID.ToString()); 
        return;
    }

    // 무기용 새 스테틱 메시 컴포넌트 생성
    UStaticMeshComponent* WeaponMeshComp = NewObject<UStaticMeshComponent>(OwnerCharacter, FName(*FString::Printf(TEXT("Weapon_Mesh_%s"), *Item->ItemID.ToString()))); 
    WeaponMeshComp->SetStaticMesh(ItemMesh); 
    WeaponMeshComp->SetCollisionProfileName(TEXT("Weapon"));
    WeaponMeshComp->SetGenerateOverlapEvents(true);
    WeaponMeshComp->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Item->SocketName);
    WeaponMeshComp->RegisterComponent();

    // 스케일 적용
    WeaponMeshComp->SetRelativeScale3D(Item->Scale);
    
    // 아이템 시각 효과 처리
    HandleItemVisualEffects(Item, true);
    
    // 무기 충돌 이벤트 등록  
    WeaponMeshComp->OnComponentBeginOverlap.AddDynamic(this, &UItemComponent::HandleWeaponCollision);
    
    // 기본적으로 충돌 비활성화 (스킬 사용시에만 활성화)
    SetWeaponCollisionEnabled(false);
}

void UItemComponent::HandleItemVisualEffects(UItemDataAsset* Item, bool bEquipping)
{
    // 아이템 장착/해제 시 시각 효과 (필요시 구현)
}

/**
 * 현재 장착된 아이템의 스킬 세트를 업데이트합니다.
 *
 * 이 메서드는 다음과 같은 작업을 수행합니다:
 * - 장착된 아이템이 없거나 스킬 매니저가 없는 경우 스킬 리스트 초기화
 * - 아이템에서 가져온 스킬 세트를 사용하여 사용 가능한 스킬 목록 업데이트
 * - 스킬 매니저에 새 스킬들을 등록
 */
void UItemComponent::UpdateAvailableSkills()
{
    if (!EquippedItem || !SkillManager)
    {
        AvailableSkills.Empty();
        return;
    }
    
    // 아이템에서 스킬 세트 가져오기
    AvailableSkills = EquippedItem->SkillSet;
    
    // 스킬 매니저에 스킬 등록
    for (const FSkillData& Skill : AvailableSkills)
    {
        SkillManager->RegisterSkill(Skill);
    }
}

/**
 * 무기 충돌을 처리합니다.
 *
 * 이 메소드는 무기의 충돌 이벤트 발생 시 호출됩니다. 충돌 대상이 자기 자신일 경우 처리를 무시하며,
 * SkillManager를 통해 충돌 효과를 처리합니다.
 *
 * @param OverlappedComponent 충돌이 발생한 프리미티브 컴포넌트
 * @param OtherActor 충돌한 대상 액터
 * @param OtherComp 충돌한 대상의 컴포넌트
 * @param OtherBodyIndex 충돌한 대상의 본 index
 * @param bFromSweep 스윕으로부터 발생한 충돌인지 여부
 * @param SweepResult 충돌에 대한 세부 정보를 담고 있는 FHitResult
 */
void UItemComponent::HandleWeaponCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 자기 자신과의 충돌 무시
    if (OtherActor == GetOwner())
    {
        return;
    }
    
    // SkillManager를 통해 충돌 처리
    if (SkillManager)
    {
        SkillManager->ProcessWeaponCollision(OverlappedComponent, OtherActor, SweepResult);
    }
}

/**
 * 무기의 충돌 활성화 또는 비활성화를 설정합니다.
 *
 * 해당 메서드는 현재 장착된 아이템에 연결된 무기의 충돌 설정을 변경합니다.
 * 아이템의 무기 메시 컴포넌트를 찾고, 충돌 이벤트와 충돌 활성 상태를
 * 입력된 값에 따라 업데이트합니다.
 *
 * @param bEnabled 충돌 설정을 활성화하려면 true, 비활성화하려면 false를 전달합니다.
 */
void UItemComponent::SetWeaponCollisionEnabled(bool bEnabled)
{
    // 소유자 캐릭터 가져오기
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        return;
    }
    
    // 무기 메시 컴포넌트 찾기
    if (EquippedItem)
    {
        UStaticMeshComponent* WeaponMeshComp = nullptr;
        TArray<USceneComponent*> ChildComponents;
        OwnerCharacter->GetMesh()->GetChildrenComponents(true, ChildComponents);

        for (USceneComponent* ChildComp : ChildComponents)
        {
            if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(ChildComp))
            {
                if (StaticMeshComp->GetName().Contains(EquippedItem->ItemID.ToString()))
                {
                    WeaponMeshComp = StaticMeshComp;
                    break;
                }
            }
        }

        
        if (WeaponMeshComp)
        {
            // 충돌 설정 변경
            WeaponMeshComp->SetGenerateOverlapEvents(bEnabled);
            
            // 물리 시뮬레이션 설정 (필요시)
            if (bEnabled)
            {
                WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            }
            else
            {
                WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
        }
    }
}

/**
 * 현재 장착된 아이템에서 사용 가능한 스킬의 수를 반환합니다.
 *
 * @return 사용 가능한 스킬의 수
 */
int32 UItemComponent::GetAvailableSkillCount() const
{
    return AvailableSkills.Num();
}

/**
 * 인덱스로 스킬 데이터를 가져옵니다.
 * 
 * @param SkillIndex 가져올 스킬의 인덱스
 * @return 스킬 데이터, 유효하지 않은 인덱스인 경우 기본 스킬 데이터 반환
 */
FSkillData UItemComponent::GetSkillData(int32 SkillIndex) const
{
    if (SkillIndex >= 0 && SkillIndex < AvailableSkills.Num())
    {
        return AvailableSkills[SkillIndex];
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
bool UItemComponent::IsSkillOnCooldown(int32 SkillIndex) const
{
    if (!SkillManager || SkillIndex < 0 || SkillIndex >= AvailableSkills.Num())
    {
        return false;
    }
    
    return SkillManager->IsSkillOnCooldown(SkillIndex);
}

/**
 * 인덱스로 스킬의 남은 쿨다운 시간을 가져옵니다.
 *
 * @param SkillIndex 확인할 스킬의 인덱스
 * @return 해당 스킬의 남은 쿨다운 시간 (초 단위)
 */
float UItemComponent::GetSkillCooldownRemaining(int32 SkillIndex) const
{
    if (!SkillManager || SkillIndex < 0 || SkillIndex >= AvailableSkills.Num())
    {
        return 0.0f;
    }
    
    return SkillManager->GetCooldownRemaining(SkillIndex);
}

/**
 * 서버에서 아이템을 장착합니다.
 *
 * 이 함수는 클라이언트가 아이템 장착을 요청했을 때 서버에서 호출되어 실행됩니다.
 * 내부적으로 `EquipItem` 메서드를 호출하여 아이템 장착 로직을 처리합니다.
 *
 * 작업 흐름:
 * - 서버는 해당 아이템 ID를 기반으로 장착 처리를 수행합니다.
 * - 서버에서의 권한을 보장하기 위해 동작합니다.
 *
 * @param ItemID 장착하려는 아이템의 고유 식별자입니다.
 */
void UItemComponent::Server_EquipItem_Implementation(FName ItemID)
{
    // 서버에서 아이템 장착 처리
    EquipItem(ItemID);
}

/**
 * 신규 아이템을 클라이언트에 장착 처리합니다.
 *
 * 다음 작업이 수행됩니다:
 * - 장착된 아이템을 업데이트
 * - 아이템을 소켓에 부착
 * - 사용 가능한 스킬 목록 갱신
 * - 아이템 장착 이벤트 발생
 *
 * @param NewItem 새로운 아이템 데이터 자산의 참조입니다.
 */
void UItemComponent::Client_OnItemEquipped_Implementation(UItemDataAsset* NewItem)
{
    // 클라이언트에서 아이템 장착 결과 처리
    if (NewItem != EquippedItem)
    {
        EquippedItem = NewItem;
        AttachItemToSocket(NewItem);
        UpdateAvailableSkills();
        OnItemEquipped.Broadcast(NewItem);
    }
}