#include "PickupableItem.h"

#include "Characters/Components/ItemComponent/ItemComponent.h"
#include "Components/SphereComponent.h"
#include "InteractionComponent/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "WeaponMaster/Characters/Components/IBattleSystemUser.h"
#include "WeaponMaster/Data/GameDataManager.h"
#include "WeaponMaster/Data/WeaponMasterLib.h"

APickupableItem::APickupableItem()
{
    // 액터 설정
    PrimaryActorTick.bCanEverTick = true;

    // 메시 컴포넌트 설정
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
    SetReplicateMovement(true); // 이동(물리) 복제도 활성화
    
    // 복제 관련 추가 설정
    MeshComponent->SetIsReplicated(true);
    InteractionSphere->SetIsReplicated(true);
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
                
                // MeshData에서 첫 번째 메시의 스케일 사용 (있는 경우)
                if (ItemData->MeshData.Num() > 0)
                {
                    MeshComponent->SetRelativeScale3D(ItemData->MeshData[0].Scale);
                }
                else
                {
                    // 기본 스케일 1.0 적용
                    MeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
                }
            }
        }
    }
    else if (ItemData)
    {
        // 아이템 데이터가 있으면 메시 설정
        if (!ItemData->ItemMesh.IsNull())
        {
            UStaticMesh* Mesh = ItemData->ItemMesh.LoadSynchronous();
            if (Mesh)
            {
                MeshComponent->SetStaticMesh(Mesh);
                
                // MeshData에서 첫 번째 메시의 스케일 사용 (있는 경우)
                if (ItemData->MeshData.Num() > 0)
                {
                    MeshComponent->SetRelativeScale3D(ItemData->MeshData[0].Scale);
                }
                else
                {
                    // 기본 스케일 1.0 적용
                    MeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
                }
            }
        }
    }
}

void APickupableItem::ProcessPickup(AActor* Interactor)
{
    // 서버에서만 처리
    if (!HasAuthority())
    {
        return;
    }
    
    ACharacter* Character = Cast<ACharacter>(Interactor);
    if (!Character || !ItemData)
    {
        return;
    }
    
    if (Character->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
    {
        bool bPickedUp = IBattleSystemUser::Execute_EquipItem(Character, ItemID);
        
        if (bPickedUp)
        {
            // 효과음 재생
            if (!ItemData->PickupSound.IsNull())
            {
                USoundBase* Sound = ItemData->PickupSound.LoadSynchronous();
                if (Sound)
                {
                    UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
                }
            }
            
            // 먼저 모든 클라이언트에게 아이템 숨김 처리
            Client_OnPickupSuccess();
            
            // 잠시 대기 후 아이템 파괴 (네트워크 동기화를 위한 지연)
            FTimerHandle DestroyTimerHandle;
            GetWorldTimerManager().SetTimer(
                DestroyTimerHandle,
                FTimerDelegate::CreateLambda([this]()
                {
                    UE_LOG(LogTemp, Warning, TEXT("[서버] 아이템 %s 파괴"), *GetName());
                    Destroy();
                }),
                0.2f,  // 0.2초 지연
                false
            );
        }
    }
}

void APickupableItem::Server_OnPickedUp_Implementation(AActor* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("Server_OnPickedUp: 서버에서 아이템 %s 획득 처리"), 
           ItemData ? *ItemData->ItemName : TEXT("Unknown"));
           
    ProcessPickup(Interactor);
}

void APickupableItem::OnPickup(AActor* Interactor)
{
    // ItemData가 없지만 ItemID가 유효하면 데이터 로드 시도
    if (!ItemData && !ItemID.IsNone())
    {
        LoadItemData();
    }
    
    // 캐릭터를 통해 아이템 획득 요청 처리
    ACharacter* Character = Cast<ACharacter>(Interactor);
    if (!Character || !ItemData)
    {
        UE_LOG(LogTemp, Error, TEXT("OnPickup: Character or ItemData is invalid"));
        return;
    }
    
    if (Character->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
    {
        // 직접 Server_OnPickedUp 호출 부분 제거하고 아래 코드만 유지
        IBattleSystemUser::Execute_RequestItemPickup(Character, this);
    }
}

void APickupableItem::Client_OnPickupSuccess_Implementation()
{
    // 클라이언트에서 시각적으로 아이템 숨김 (보이지 않게 처리)
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}

void APickupableItem::OnInteractionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                       bool bFromSweep, const FHitResult& SweepResult)
{
    // 캐릭터 확인
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character)
    {
        return;
    }
    
    if (Character->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
    {
        // 상호작용 액터 설정
        IBattleSystemUser::Execute_SetInteractableActor(Character, this);
        
        // 상호작용 UI 표시 - 텍스트 설정 호출 제거
        UInteractionComponent* InteractionComp = FindComponentByClass<UInteractionComponent>();
        if (InteractionComp)
        {
            // 텍스트 설정 함수 호출 제거
            // 위젯만 활성화
            InteractionComp->EnableInteraction(Character);
        }
    }
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

    if (Character->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
    {
        if (IBattleSystemUser::Execute_GetInteractableActor(Character))
        {
            IBattleSystemUser::Execute_SetInteractableActor(Character, nullptr);
        }
        // 상호작용 UI 숨기기
        InteractionComponent->DisableInteraction();
    }
}

void APickupableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(APickupableItem, ItemID);
    DOREPLIFETIME(APickupableItem, ItemData);
}