#include "GameDataManager.h"
#include "Engine/AssetManager.h"
#include "Engine/ObjectLibrary.h"
#include "UObject/UObjectIterator.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/LogMacros.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogGameData, Log, All);

UGameDataManager::UGameDataManager()
{
    ContentBasePath = TEXT("/Game/WeaponMaster/Blueprints/Items/DataAsset/");
}

void UGameDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    ContentBasePath = TEXT("/Game/WeaponMaster/Blueprints/Items/DataAsset/");
    LoadAllGameData();

    UE_LOG(LogGameData, Log, TEXT("GameDataManager Subsystem Initialized"));
}

void UGameDataManager::Deinitialize()
{
    ClearCache();
    UE_LOG(LogGameData, Log, TEXT("GameDataManager Subsystem Deinitialized"));

    Super::Deinitialize();
}


UItemDataAsset* UGameDataManager::GetItemData(FName ItemID)
{
    // 캐시에서 아이템 찾기
    UItemDataAsset** FoundItem = ItemDataCache.Find(ItemID);
    if (FoundItem && *FoundItem)
    {
        return *FoundItem;
    }

    FSoftObjectPath ObjectPath(ContentBasePath + ItemID.ToString() + TEXT(".") + ItemID.ToString());

    // 결과를 저장할 FAssetData 변수 선언
    FAssetData AssetData;

    // 함수 호출 (반환값은 성공 여부를 나타내는 bool 값)
    bool bFoundAsset = UAssetManager::Get().GetAssetDataForPath(ObjectPath, AssetData);

    if (bFoundAsset)
    {
        UItemDataAsset* LoadedItem = Cast<UItemDataAsset>(AssetData.GetAsset());
        if (LoadedItem)
        {
            // 캐시에 추가
            UpdateItemCache(LoadedItem);
            return LoadedItem;
        }
    }

    UE_LOG(LogGameData, Warning, TEXT("Failed to find item data for ID: %s"), *ItemID.ToString());
    return nullptr;
}

void UGameDataManager::LoadItemDataAsync(FName ItemID, FName CallbackFunctionName, UObject* CallbackTarget)
{
    // 캐시에서 아이템 찾기
    UItemDataAsset** FoundItem = ItemDataCache.Find(ItemID);
    if (FoundItem && *FoundItem)
    {
        // 이미 로드된 경우 바로 콜백 실행
        OnItemDataLoaded(ItemID, *FoundItem, CallbackFunctionName, CallbackTarget);
        return;
    }

    // 에셋 매니저를 통해 비동기 로드
    FString ItemPath = ContentBasePath + ItemID.ToString(); // 아이템 경로 수정
    FSoftObjectPath AssetPath(ItemPath);

    FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

    // 이미 진행 중인 로드 요청이 있는지 확인
    if (LoadRequests.Contains(ItemID))
    {
        UE_LOG(LogGameData, Warning, TEXT("Already loading item data for ID: %s"), *ItemID.ToString());
        return;
    }

    // 비동기 로드 요청 생성
    FStreamableDelegate LoadDelegate = FStreamableDelegate::CreateUObject(
        this,
        &UGameDataManager::OnItemDataLoaded,
        ItemID,
        (UItemDataAsset*)nullptr, // 로드 결과는 콜백에서 처리
        CallbackFunctionName,
        CallbackTarget
    );

    // 로드 요청 추가
    LoadRequests.Add(ItemID, LoadDelegate);

    // 비동기 로드 시작
    StreamableManager.RequestAsyncLoad(
        AssetPath,
        LoadDelegate
    );
}

TArray<UItemDataAsset*> UGameDataManager::GetAllItemData()
{
    TArray<UItemDataAsset*> AllItems;
    ItemDataCache.GenerateValueArray(AllItems);
    return AllItems;
}

void UGameDataManager::SetContentPath(const FString& Path)
{
    ContentBasePath = Path;
    RefreshGameData();
}

void UGameDataManager::RefreshGameData()
{
    // 캐시 정리
    ClearCache();

    // 데이터 다시 로드
    LoadAllGameData();
}

void UGameDataManager::ClearCache()
{
    // 아이템 캐시 정리
    ItemDataCache.Empty();

    // 로드 요청 정리
    for (auto& Request : LoadRequests)
    {
        // 필요시 취소 로직 추가
    }
    LoadRequests.Empty();
}

void UGameDataManager::LoadAllGameData()
{
    // 아이템 데이터 로드
    LoadItemsFromPath(ContentBasePath + TEXT("/Items"));

    UE_LOG(LogGameData, Log, TEXT("Loaded %d items"), ItemDataCache.Num());
}

void UGameDataManager::LoadItemsFromPath(const FString& Path)
{
    // 오브젝트 라이브러리 생성
    UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UItemDataAsset::StaticClass(), true, GIsEditor);
    if (!ObjectLibrary)
    {
        UE_LOG(LogGameData, Error, TEXT("Failed to create object library for items"));
        return;
    }

    // 경로에서 에셋 로드
    ObjectLibrary->LoadAssetDataFromPath(Path);

    // 모든 에셋 데이터 가져오기
    TArray<FAssetData> AssetDatas;
    ObjectLibrary->GetAssetDataList(AssetDatas);

    // 에셋 로드 및 캐싱
    for (const FAssetData& AssetData : AssetDatas)
    {
        UItemDataAsset* ItemData = Cast<UItemDataAsset>(AssetData.GetAsset());
        if (ItemData)
        {
            UpdateItemCache(ItemData);
        }
    }
}


void UGameDataManager::UpdateItemCache(UItemDataAsset* ItemData)
{
    if (!ItemData || ItemData->ItemID == NAME_None)
    {
        return;
    }

    // 캐시에 아이템 추가/업데이트
    ItemDataCache.Add(ItemData->ItemID, ItemData);
    
}

void UGameDataManager::OnItemDataLoaded(FName ItemID, UItemDataAsset* LoadedItem, FName CallbackFunctionName, UObject* CallbackTarget)
{
    // 로드 요청 제거
    LoadRequests.Remove(ItemID);

    // LoadedItem이 nullptr이면 로드 결과를 가져와야 함
    if (!LoadedItem)
    {
        FString ItemPath = ContentBasePath + TEXT("/Items/") + ItemID.ToString(); // 아이템 경로 수정
        FSoftObjectPath AssetPath(ItemPath);
        LoadedItem = Cast<UItemDataAsset>(AssetPath.TryLoad());
    }

    // 로드된 아이템 캐싱
    if (LoadedItem)
    {
        UpdateItemCache(LoadedItem);
    }

    // 콜백 실행
    if (CallbackTarget && !CallbackFunctionName.IsNone())
    {
        // UFunction을 사용하여 콜백 함수 호출
        UFunction* CallbackFunction = CallbackTarget->FindFunction(CallbackFunctionName);
        if (CallbackFunction)
        {
            struct {
                FName ItemID;
                UItemDataAsset* ItemData;
            } Params;

            Params.ItemID = ItemID;
            Params.ItemData = LoadedItem;

            CallbackTarget->ProcessEvent(CallbackFunction, &Params);
        }
    }
}