// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemComponent.h"

#include "NiagaraComponent.h"
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
    
    // 메시 데이터 체크
    if (NewItem->MeshData.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] Item %s has %d mesh data entries"), 
            *NewItem->ItemName, NewItem->MeshData.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] Item %s has no mesh data!"), *NewItem->ItemName);
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
                // NULL 체크 추가
                if (StaticMeshComp && !PreviousItem->ItemID.IsNone() && StaticMeshComp->GetName().Contains(PreviousItem->ItemID.ToString()))
                {
                    UE_LOG(LogTemp, Warning, TEXT("[SERVER] Found weapon mesh component, will destroy: %s"), *StaticMeshComp->GetName());
                    StaticMeshComp->DestroyComponent();
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
    
    // 모든 클라이언트에 알림 (아이템이 변경되었음을 알림)
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        // 클라이언트에게 알림 - 오류 방지를 위해 NULL 체크 추가
        if (NewItem)
        {
            Client_OnItemEquipped(NewItem);
        }
    }
    
    // 아이템 메시 비동기 로드 필요 확인
    bool bNeedsLoading = false;
    
    // MeshData 구조 확인
    if (NewItem && NewItem->MeshData.Num() > 0)
    {
        for (const FItemMeshData& MeshData : NewItem->MeshData)
        {
            if (!MeshData.Mesh.IsNull())
            {
                bNeedsLoading = true;
                break;
            }
        }
    }
    
    if (bNeedsLoading)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] Loading meshes asynchronously for item: %s"), *NewItem->ItemName);
        
        // 비동기 로드 시작
        UAssetManager& AssetManager = UAssetManager::Get();
        FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
        
        // 로드할 메시 목록 생성
        TArray<FSoftObjectPath> AssetsToLoad;
        
        // MeshData 구조에서 에셋 추가
        for (const FItemMeshData& MeshData : NewItem->MeshData)
        {
            if (!MeshData.Mesh.IsNull())
            {
                AssetsToLoad.Add(MeshData.Mesh.ToSoftObjectPath());
            }
            if (!MeshData.Effect.IsNull())
            {
                AssetsToLoad.Add(MeshData.Effect.ToSoftObjectPath());
            }
        }
        
        LoadHandle = StreamableManager.RequestAsyncLoad(
            AssetsToLoad,
            FStreamableDelegate::CreateUObject(this, &UItemComponent::OnMeshLoadCompleted)
        );
        
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] No meshes to load for item: %s"), NewItem ? *NewItem->ItemName : TEXT("INVALID"));
        
        // 메시가 없는 경우 바로 장착 완료
        if (NewItem)
        {
            AttachItemToSocket(NewItem);
            OnItemEquipped.Broadcast(NewItem);
            
            // 클라이언트에게 알림
            Client_OnItemEquipped(NewItem);
        }
        
        return NewItem != nullptr;
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

    UE_LOG(LogTemp, Warning, TEXT("[%s] 아이템 %s 해제 시도 (권한: %d)"), 
    GetOwnerRole() == ROLE_Authority ? TEXT("서버") : TEXT("클라이언트"),
    EquippedItem ? *EquippedItem->ItemName : TEXT("없음"),
    (int32)GetOwnerRole());
    
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
    
    checkf(PickupItemClass, TEXT("[UItemComponent::SpawnPickupItem] PickupItemClass is not set"));
    
    // Spawn the pickup item actor
    APickupableItem* PickupItem = World->SpawnActor<APickupableItem>(PickupItemClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    
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
    if (!ItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to start async load for item %s: Item data not found"), *ItemID.ToString());
        return;
    }
    
    // 아이템의 메시 데이터가 있는지 확인
    if (ItemData->MeshData.Num() == 0 || ItemData->MeshData[0].Mesh.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to start async load for item %s: No mesh data available"), *ItemID.ToString());
        return;
    }
    
    // 비동기 로드 시작
    UAssetManager& AssetManager = UAssetManager::Get();
    FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
    
    TArray<FSoftObjectPath> ItemsToLoad;
    
    // 모든 메시와 이펙트를 로드 목록에 추가
    for (const FItemMeshData& MeshData : ItemData->MeshData)
    {
        if (!MeshData.Mesh.IsNull())
        {
            ItemsToLoad.Add(MeshData.Mesh.ToSoftObjectPath());
        }
        
        if (!MeshData.Effect.IsNull())
        {
            ItemsToLoad.Add(MeshData.Effect.ToSoftObjectPath());
        }
    }
    
    if (ItemsToLoad.Num() > 0)
    {
        LoadHandle = StreamableManager.RequestAsyncLoad(
            ItemsToLoad,
            FStreamableDelegate::CreateUObject(this, &UItemComponent::OnItemLoaded, ItemID)
        );
        
        UE_LOG(LogTemp, Warning, TEXT("Started async load for item %s with %d assets"), 
            *ItemID.ToString(), ItemsToLoad.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No assets to load for item %s"), *ItemID.ToString());
    }
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
    UE_LOG(LogTemp, Warning, TEXT("[%s] 아이템 %s의 메시 로드 완료"), 
        GetOwnerRole() == ROLE_Authority ? TEXT("서버") : TEXT("클라이언트"),
        EquippedItem ? *EquippedItem->ItemName : TEXT("Unknown"));
    
    // 로드 완료 후 처리
    if (EquippedItem)
    {
        // 아이템 소켓에 부착
        AttachItemToSocket(EquippedItem);
        
        // 이벤트 발생
        OnItemEquipped.Broadcast(EquippedItem);
        
        // 모든 클라이언트에게 알림
        if (GetOwnerRole() == ROLE_Authority)
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
        UE_LOG(LogTemp, Error, TEXT("AttachItemToSocket: NULL 아이템 전달됨"));
        return;
    }
    
    // 소유자 캐릭터 가져오기
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("AttachItemToSocket: 소유자 캐릭터가 NULL"));
        return;
    }
    
    // 메시 컴포넌트에 접근
    USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh();
    if (!MeshComp)
    {
        UE_LOG(LogTemp, Error, TEXT("AttachItemToSocket: 메시 컴포넌트가 NULL"));
        return;
    }
    
    // 기존 무기 메시 컴포넌트 및 이펙트 컴포넌트 제거 (중복 방지)
    TArray<USceneComponent*> ChildComponents;
    MeshComp->GetChildrenComponents(true, ChildComponents);
    for (USceneComponent* Child : ChildComponents)
    {
        UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(Child);
        if (StaticMeshComp && StaticMeshComp->GetName().Contains(TEXT("Weapon_Mesh")))
        {
            StaticMeshComp->DestroyComponent();
        }
        
        UNiagaraComponent* NiagaraComp = Cast<UNiagaraComponent>(Child);
        if (NiagaraComp && NiagaraComp->GetName().Contains(TEXT("Weapon_Effect")))
        {
            NiagaraComp->DestroyComponent();
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[%s] 아이템 %s 부착 시작 (권한: %d)"), 
        GetOwner()->HasAuthority() ? TEXT("서버") : TEXT("클라이언트"),
        *Item->ItemName,
        (int32)GetOwnerRole());

    // MeshData 배열에 데이터가 있는지 확인
    if (Item->MeshData.Num() > 0)
    {
        // 각 MeshData 항목을 순회하며 부착
        for (int32 i = 0; i < Item->MeshData.Num(); ++i)
        {
            const FItemMeshData& MeshData = Item->MeshData[i];
            
            // 메시가 할당되지 않은 경우 건너뛰기
            if (MeshData.Mesh.IsNull())
            {
                UE_LOG(LogTemp, Warning, TEXT("아이템 %s의 메시 데이터 %d: 메시가 NULL"), *Item->ItemName, i);
                continue;
            }
            
            // 메시 로드
            UStaticMesh* ItemMesh = MeshData.Mesh.Get();
            if (!ItemMesh)
            {
                UE_LOG(LogTemp, Warning, TEXT("아이템 %s의 메시 %d 로드 실패"), *Item->ItemID.ToString(), i); 
                continue;
            }
            
            UE_LOG(LogTemp, Warning, TEXT("[%s] 아이템 %s의 메시 %d를 소켓 %s에 부착 (권한: %d)"), 
                GetOwner()->HasAuthority() ? TEXT("서버") : TEXT("클라이언트"),
                *Item->ItemName, 
                i,
                *MeshData.SocketName.ToString(),
                (int32)GetOwnerRole());
            
            // 복제 가능한 무기 메시 컴포넌트 생성
            FName ComponentName = FName(*FString::Printf(TEXT("Weapon_Mesh_%s_%d"), *Item->ItemID.ToString(), i));
            UStaticMeshComponent* WeaponMeshComp = NewObject<UStaticMeshComponent>(OwnerCharacter, ComponentName);
            WeaponMeshComp->SetStaticMesh(ItemMesh);
            WeaponMeshComp->SetCollisionProfileName(TEXT("Weapon"));
            WeaponMeshComp->SetGenerateOverlapEvents(true);
            WeaponMeshComp->SetIsReplicated(true); // 복제 활성화
            WeaponMeshComp->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, MeshData.SocketName);
            WeaponMeshComp->RegisterComponent();
            
            // 스케일 적용
            WeaponMeshComp->SetRelativeScale3D(MeshData.Scale);
            
            // 무기 충돌 이벤트 등록
            WeaponMeshComp->OnComponentBeginOverlap.AddDynamic(this, &UItemComponent::HandleWeaponCollision);
            
            // 기본적으로 충돌 비활성화
            WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            
            // 아이템에 이펙트 추가 (이펙트가 설정된 경우만)
            if (!MeshData.Effect.IsNull())
            {
                UNiagaraSystem* EffectSystem = MeshData.Effect.LoadSynchronous();
                if (EffectSystem)
                {
                    // 이펙트 컴포넌트 생성 및 설정
                    FName EffectName = FName(*FString::Printf(TEXT("Weapon_Effect_%s_%d"), *Item->ItemID.ToString(), i));
                    UNiagaraComponent* EffectComp = NewObject<UNiagaraComponent>(OwnerCharacter, EffectName);
                    EffectComp->SetAsset(EffectSystem);
                    EffectComp->SetIsReplicated(true); // 복제 활성화
                    
                    // 무기 메시에 이펙트 부착
                    EffectComp->AttachToComponent(WeaponMeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                    
                    // 이펙트 위치 오프셋 적용
                    EffectComp->SetRelativeLocation(MeshData.EffectOffset);
                    
                    // 이펙트 스케일 적용
                    EffectComp->SetRelativeScale3D(MeshData.EffectScale);
                    
                    // 색상 파라미터 설정 (파라미터 이름이 있는 경우)
                    if (!MeshData.EffectColorParameterName.IsNone())
                    {
                        EffectComp->SetVariableLinearColor(MeshData.EffectColorParameterName, MeshData.EffectColor);
                    }
                    
                    // 컴포넌트 등록 및 활성화
                    EffectComp->RegisterComponent();
                    EffectComp->Activate(true);
                    
                    UE_LOG(LogTemp, Warning, TEXT("아이템 %s의 메시 %d에 이펙트 추가 완료"), *Item->ItemName, i);
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("아이템 %s: 메시 데이터가 없음"), *Item->ItemName);
    }
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
    
    // 장착된 아이템이 없으면 반환
    if (!EquippedItem)
    {
        return;
    }
    
    // 모든 무기 메시 컴포넌트 찾기
    TArray<UStaticMeshComponent*> WeaponMeshComps;
    TArray<USceneComponent*> ChildComponents;
    OwnerCharacter->GetMesh()->GetChildrenComponents(true, ChildComponents);

    for (USceneComponent* ChildComp : ChildComponents)
    {
        if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(ChildComp))
        {
            if (StaticMeshComp->GetName().Contains(EquippedItem->ItemID.ToString()))
            {
                WeaponMeshComps.Add(StaticMeshComp);
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("무기 충돌 %s: %d개의 메시 컴포넌트 찾음"), 
        bEnabled ? TEXT("활성화") : TEXT("비활성화"), 
        WeaponMeshComps.Num());

    // 모든 무기 메시에 충돌 설정 적용
    for (UStaticMeshComponent* WeaponMeshComp : WeaponMeshComps)
    {
        // 충돌 설정 변경
        WeaponMeshComp->SetGenerateOverlapEvents(bEnabled);
        
        // 물리 시뮬레이션 설정
        if (bEnabled)
        {
            WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            UE_LOG(LogTemp, Warning, TEXT("무기 컴포넌트 %s의 충돌 활성화"), *WeaponMeshComp->GetName());
        }
        else
        {
            WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            UE_LOG(LogTemp, Warning, TEXT("무기 컴포넌트 %s의 충돌 비활성화"), *WeaponMeshComp->GetName());
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
    if (ItemID.IsNone())
    {
        UE_LOG(LogTemp, Error, TEXT("[SERVER] RPC received invalid item ID"));
        return;
    }
    
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