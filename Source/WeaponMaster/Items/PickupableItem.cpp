#include "PickupableItem.h"
#include "Components/SphereComponent.h"
#include "InteractionComponent/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "WeaponMaster/Characters/Components/IBattleSystemUser.h"
#include "WeaponMaster/Data/GameDataManager.h"
#include "WeaponMaster/Data/WeaponMasterLib.h"

APickupableItem::APickupableItem()
{
    // 루트 컴포넌트로 메시 생성
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;
    
    // 물리 설정 (바닥에 떨어져 있도록)
    MeshComponent->SetSimulatePhysics(true);
    MeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
    
    // 상호작용 영역을 위한 스피어 컴포넌트
    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(RootComponent);
    InteractionSphere->SetSphereRadius(150.0f);
    InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    InteractionSphere->SetGenerateOverlapEvents(true);
    
    // 상호작용 컴포넌트 생성
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
    
    // 네트워크 복제 설정
    bReplicates = true;
}

void APickupableItem::BeginPlay()
{
    Super::BeginPlay();
    
    // 아이템 데이터 로드
    LoadItemData();
    
    // 상호작용 프롬프트 설정
    if (ItemData)
    {
        FString PromptText = FString::Printf(TEXT("Press F to Pick Up %s"), *ItemData->ItemName);
        InteractionComponent->SetInteractionPrompt(FText::FromString(PromptText));
    }
    
    // 상호작용 이벤트 바인딩
    InteractionComponent->OnInteract.AddDynamic(this, &APickupableItem::OnPickup);
    
    // 오버랩 이벤트 바인딩
    InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &APickupableItem::OnInteractionBegin);
    InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &APickupableItem::OnInteractionEnd);
}

void APickupableItem::LoadItemData()
{
    // ItemID가 유효하면 아이템 데이터 로드
    if (!ItemID.IsNone() && !ItemData)
    {
        UGameDataManager* GameDataManager = UWeaponMasterLib::GetGameDataManager(this);
        if (!GameDataManager)
        {
            return;
        }
        
        ItemData = GameDataManager->GetItemData(ItemID);
        
        // 아이템 데이터가 있으면 메시 설정
        if (ItemData && !ItemData->ItemMesh.IsNull())
        {
            UStaticMesh* Mesh = ItemData->ItemMesh.LoadSynchronous();
            if (Mesh)
            {
                MeshComponent->SetStaticMesh(Mesh);
                MeshComponent->SetRelativeScale3D(ItemData->Scale);
            }
        }
    }else if (ItemData)
    {
        // 아이템 데이터가 있으면 메시 설정
        if (!ItemData->ItemMesh.IsNull())
        {
            UStaticMesh* Mesh = ItemData->ItemMesh.LoadSynchronous();
            if (Mesh)
            {
                MeshComponent->SetStaticMesh(Mesh);
                MeshComponent->SetRelativeScale3D(ItemData->Scale);
            }
        }
    }
}

void APickupableItem::OnPickup(AActor* Interactor)
{
    // ACharacter로 캐스팅 후 IBattleSystemUser 인터페이스 확인
    ACharacter* Character = Cast<ACharacter>(Interactor);
    if (!Character || !ItemData)
    {
        return;
    }
    
    // 캐릭터가 IBattleSystemUser 인터페이스를 구현하는지 확인
    IBattleSystemUser* BattleSystemUser = Cast<IBattleSystemUser>(Character);
    if (!BattleSystemUser)
    {
        UE_LOG(LogTemp, Warning, TEXT("Character does not implement IBattleSystemUser"));
        return;
    }
    
    // 아이템 장착 시도
    bool bPickedUp = BattleSystemUser->EquipItem(ItemID);
    
    if (bPickedUp)
    {
        // 획득 효과음 재생
        if (!ItemData->PickupSound.IsNull())
        {
            USoundBase* Sound = ItemData->PickupSound.LoadSynchronous();
            if (Sound)
            {
                UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
            }
        }
        
        // 아이템 제거
        Destroy();
    }
}

void APickupableItem::OnInteractionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                        bool bFromSweep, const FHitResult& SweepResult)
{
    // ACharacter로 캐스팅 후 IBattleSystemUser 인터페이스 확인
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character)
    {
        return;
    }
    
    // 캐릭터가 IBattleSystemUser 인터페이스를 구현하는지 확인
    IBattleSystemUser* BattleSystemUser = Cast<IBattleSystemUser>(Character);
    if (!BattleSystemUser)
    {
        return;
    }
    
    // 상호작용 UI 표시
    InteractionComponent->EnableInteraction(Character);
    
    // 캐릭터에게 현재 상호작용 가능한 아이템 알림
    BattleSystemUser->SetInteractableActor(this);
}

void APickupableItem::OnInteractionEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // ACharacter로 캐스팅 후 IBattleSystemUser 인터페이스 확인
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character)
    {
        return;
    }
    
    // 캐릭터가 IBattleSystemUser 인터페이스를 구현하는지 확인
    IBattleSystemUser* BattleSystemUser = Cast<IBattleSystemUser>(Character);
    if (!BattleSystemUser)
    {
        return;
    }
    
    // 상호작용 UI 숨기기
    InteractionComponent->DisableInteraction();
    
    // 캐릭터의 상호작용 가능한 아이템 제거
    if (BattleSystemUser->GetInteractableActor() == this)
    {
        BattleSystemUser->SetInteractableActor(nullptr);
    }
}