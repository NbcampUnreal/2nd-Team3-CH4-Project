// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemComponent.h"

#include "Net/UnrealNetwork.h"
#include "WeaponMaster/Characters/TestCharacter.h"
#include "WeaponMaster/Characters/WeaponMasterCharacter.h"
#include "WeaponMaster/Data/GameDataManager.h"
#include "WeaponMaster/Data/ItemDataAsset.h"
#include "WeaponMaster/Data/WeaponMasterLib.h"
#include "WeaponMaster/Items/PickupableItem.h"


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
    
    
    // 소유자 캐릭터 가져오기
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        // 초기 설정
    }
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UItemComponent, EquippedItem);
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
        UE_LOG(LogTemp, Warning, TEXT("[CLIENT] Requesting server to equip item: %s"), *ItemID.ToString());
        Server_EquipItem(ItemID);
        return true; // 클라이언트는 서버 응답을 기다림
    }
    
    UE_LOG(LogTemp, Warning, TEXT("[SERVER] Equipping item: %s"), *ItemID.ToString());
    
    // 현재 아이템과 동일하면 무시
    if (EquippedItem && EquippedItem->ItemID == ItemID)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] Item already equipped: %s"), *ItemID.ToString());
        return false;
    }
    
    // 이전 아이템 추적
    UItemDataAsset* PreviousItem = nullptr;
    if (EquippedItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] Previous item found, will unequip: %s"), *EquippedItem->ItemName);
        PreviousItem = EquippedItem;
    }
    
    // GameDataManager에서 새 아이템 정보 가져오기
    UGameDataManager* GameDataManager = UWeaponMasterLib::GetGameDataManager(GetWorld());
    if (!GameDataManager)
    {
        UE_LOG(LogTemp, Error, TEXT("[SERVER] GameDataManager not found"));
        return false;
    }
    
    UItemDataAsset* NewItem = GameDataManager->GetItemData(ItemID);
    if (!NewItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] Item with ID %s not found"), *ItemID.ToString());
        return false;
    }
    
    // 이전 아이템 처리 - 아이템 스폰 등
    if (PreviousItem)
    {
        // 무기 컴포넌트 찾기 및 제거
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            TArray<USceneComponent*> ChildComponents;
            OwnerCharacter->GetMesh()->GetChildrenComponents(true, ChildComponents);
            
            for (USceneComponent* ChildComp : ChildComponents)
            {
                UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(ChildComp);
                if (StaticMeshComp && StaticMeshComp->GetName().Contains(PreviousItem->ItemID.ToString()))
                {
                    UE_LOG(LogTemp, Warning, TEXT("[SERVER] Found weapon mesh component, will destroy: %s"), *StaticMeshComp->GetName());
                    StaticMeshComp->DestroyComponent();
                    break;
                }
            }
        }
        
        // 이전 아이템 스폰
        SpawnPickupItem(PreviousItem);
        
        // 이벤트 발생
        OnItemUnequipped.Broadcast();
    }
    
    // 이제 새 아이템 설정
    EquippedItem = NewItem;
    
    // 아이템 메시 비동기 로드
    if (!NewItem->EquippedMesh.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] Loading mesh asynchronously for item: %s"), *NewItem->ItemName);
        
        // 비동기 로드 시작
        UAssetManager& AssetManager = UAssetManager::Get();
        FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
        
        LoadHandle = StreamableManager.RequestAsyncLoad(
            NewItem->EquippedMesh.ToSoftObjectPath(),
            FStreamableDelegate::CreateUObject(this, &UItemComponent::OnMeshLoadCompleted)
        );
        
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] No mesh to load for item: %s"), *NewItem->ItemName);
        
        // 메시가 없는 경우 바로 장착 완료
        AttachItemToSocket(NewItem);
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
    
    // 로그 추가
    UE_LOG(LogTemp, Warning, TEXT("Unequipping item: %s"), *EquippedItem->ItemName);
    
    // Store reference to the item being unequipped
    UItemDataAsset* ItemToSpawn = EquippedItem;
    
    // Handle visual effects for unequipping
    HandleItemVisualEffects(EquippedItem, false);
    
    // Disable weapon collision
    SetWeaponCollisionEnabled(false);
    
    // 클라이언트에서 호출된 경우 서버에 요청
    if (GetOwnerRole() < ROLE_Authority)
    {
        Server_UnequipItem(ItemToSpawn->ItemID);
    }
    else
    {
        // 서버에서는 아이템 스폰 직접 처리
        SpawnPickupItem(ItemToSpawn);
    }
    
    // Broadcast unequip event
    OnItemUnequipped.Broadcast();
    
    // Clear item reference
    EquippedItem = nullptr;
}


void UItemComponent::SpawnPickupItem(UItemDataAsset* ItemData)
{
    if (!ItemData)
    {
        UE_LOG(LogTemp, Error, TEXT("[SERVER] Cannot spawn pickup: ItemData is null"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("[SERVER] Attempting to spawn pickup item: %s"), *ItemData->ItemName);
    
    // Get owner character
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("[SERVER] Cannot spawn pickup: Owner character is invalid"));
        return;
    }
    
    // Get the world
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("[SERVER] Cannot spawn pickup: World is invalid"));
        return;
    }
    
    // Calculate spawn location (in front of the character)
    FVector CharacterLocation = OwnerCharacter->GetActorLocation();
    FVector CharacterForward = OwnerCharacter->GetActorForwardVector();
    FVector SpawnLocation = CharacterLocation + (CharacterForward * 100.0f); // 100 units in front of character
    
    // Add a small random offset
    SpawnLocation.X += FMath::RandRange(-20.0f, 20.0f);
    SpawnLocation.Y += FMath::RandRange(-20.0f, 20.0f);
    SpawnLocation.Z += 50.0f; // Spawn a bit above ground level
    
    // Prepare spawn parameters
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = OwnerCharacter;
    
    // Spawn the pickup item actor
    APickupableItem* PickupItem = World->SpawnActor<APickupableItem>(APickupableItem::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    
    if (PickupItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] Successfully spawned pickup item: %s"), *ItemData->ItemName);
        
        // Set the item ID and data
        PickupItem->ItemID = ItemData->ItemID;
        
        // Load the mesh
        PickupItem->LoadItemData();
        
        // Add impulse for a bit of physical movement
        if (PickupItem->MeshComponent && PickupItem->MeshComponent->IsSimulatingPhysics())
        {
            FVector RandomImpulse = FVector(FMath::RandRange(-100.0f, 100.0f), FMath::RandRange(-100.0f, 100.0f), FMath::RandRange(50.0f, 150.0f));
            PickupItem->MeshComponent->AddImpulse(RandomImpulse);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[SERVER] Failed to spawn pickup item"));
    }
}

// 서버 RPC 함수 구현
void UItemComponent::Server_UnequipItem_Implementation(FName ItemID)
{
    UE_LOG(LogTemp, Warning, TEXT("Server RPC: Unequipping item %s"), *ItemID.ToString());
    
    // 아이템 ID로 데이터 가져오기
    UGameDataManager* GameDataManager = UWeaponMasterLib::GetGameDataManager(GetWorld());
    if (GameDataManager)
    {
        UItemDataAsset* ItemToSpawn = GameDataManager->GetItemData(ItemID);
        if (ItemToSpawn)
        {
            SpawnPickupItem(ItemToSpawn);
        }
    }
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
    UE_LOG(LogTemp, Warning, TEXT("[SERVER] Mesh load completed for item: %s"), EquippedItem ? *EquippedItem->ItemName : TEXT("Unknown"));
    
    // 로드 완료 후 처리
    if (EquippedItem)
    {
        // 아이템 소켓에 부착
        AttachItemToSocket(EquippedItem);
        
        // 이벤트 발생
        OnItemEquipped.Broadcast(EquippedItem);
        
        // 클라이언트에게 알림
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter && OwnerCharacter->GetLocalRole() == ROLE_Authority)
        {
            Client_OnItemEquipped(EquippedItem);
        }
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
    
    // 무기 충돌 이벤트 등록  // 사용위치 확인 필요
    WeaponMeshComp->OnComponentBeginOverlap.AddDynamic(this, &UItemComponent::HandleWeaponCollision);
    
    // 기본적으로 충돌 비활성화 (스킬 사용시에만 활성화)
    SetWeaponCollisionEnabled(false);
}

void UItemComponent::HandleItemVisualEffects(UItemDataAsset* Item, bool bEquipping)
{
    // 아이템 장착/해제 시 시각 효과 (필요시 구현)
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

    UE_LOG(LogTemp, Warning, TEXT("[UItemComponent::HandleWeaponCollision]에서 오버랩됨"));
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
    UE_LOG(LogTemp, Warning, TEXT("[SERVER] RPC received to equip item: %s"), *ItemID.ToString());
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
    if (!NewItem)
    {
        UE_LOG(LogTemp, Error, TEXT("[CLIENT] Received null item in Client_OnItemEquipped"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("[CLIENT] Equipping item: %s"), *NewItem->ItemName);
    
    // 이전 아이템 정리 (클라이언트 측)
    if (EquippedItem && EquippedItem != NewItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CLIENT] Clearing previous item: %s"), *EquippedItem->ItemName);
        
        // 무기 컴포넌트 찾기 및 제거
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            TArray<USceneComponent*> ChildComponents;
            OwnerCharacter->GetMesh()->GetChildrenComponents(true, ChildComponents);
            
            for (USceneComponent* ChildComp : ChildComponents)
            {
                UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(ChildComp);
                if (StaticMeshComp && StaticMeshComp->GetName().Contains(EquippedItem->ItemID.ToString()))
                {
                    UE_LOG(LogTemp, Warning, TEXT("[CLIENT] Found weapon mesh component, will destroy: %s"), *StaticMeshComp->GetName());
                    StaticMeshComp->DestroyComponent();
                    break;
                }
            }
        }
        
        // 이벤트 발생
        OnItemUnequipped.Broadcast();
    }
    
    // 새 아이템 설정
    EquippedItem = NewItem;
    
    // 아이템을 소켓에 부착
    AttachItemToSocket(NewItem);
    
    // 이벤트 발생
    OnItemEquipped.Broadcast(NewItem);
}