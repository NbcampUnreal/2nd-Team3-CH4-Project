#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h" // UGameInstanceSubsystem 헤더 포함
#include "UObject/NoExportTypes.h"
#include "ItemDataAsset.h"
#include "Engine/AssetManager.h"
#include "GameDataManager.generated.h"

/**
 *
 */
UCLASS()
class WEAPONMASTER_API UGameDataManager : public UGameInstanceSubsystem // UGameInstanceSubsystem 상속
{
	GENERATED_BODY()

public:
	UGameDataManager();

    // UGameInstanceSubsystem overrides
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

	// 아이템 데이터 가져오기
	UFUNCTION(BlueprintCallable, Category = "Game Data")
	UItemDataAsset* GetItemData(FName ItemID);

	// 아이템 데이터 비동기 로드
	UFUNCTION(BlueprintCallable, Category = "Game Data")
	void LoadItemDataAsync(FName ItemID, FName CallbackFunctionName, UObject* CallbackTarget);

	// 아이템 데이터 목록 가져오기
	UFUNCTION(BlueprintCallable, Category = "Game Data")
	TArray<UItemDataAsset*> GetAllItemData();
	
	// 게임 콘텐츠 경로 설정
	UFUNCTION(BlueprintCallable, Category = "Game Data")
	void SetContentPath(const FString& Path);

	// 데이터 갱신
	UFUNCTION(BlueprintCallable, Category = "Game Data")
	void RefreshGameData();

	// 데이터 캐시 정리
	UFUNCTION(BlueprintCallable, Category = "Game Data")
	void ClearCache();

protected:
	// 모든 게임 데이터 로드
	UFUNCTION()
	void LoadAllGameData();

	// 특정 경로의 모든 아이템 데이터 로드
	UFUNCTION()
	void LoadItemsFromPath(const FString& Path);

	// 아이템 캐시 업데이트
	UFUNCTION()
	void UpdateItemCache(UItemDataAsset* ItemData);


private:

	// 아이템 데이터 캐시
	UPROPERTY()
	TMap<FName, UItemDataAsset*> ItemDataCache;

	// 콘텐츠 기본 경로
	UPROPERTY()
	FString ContentBasePath;

	// 비동기 로드 요청 목록
	TMap<FName, FStreamableDelegate> LoadRequests;

	// 비동기 로드 콜백 함수
	UFUNCTION()
	void OnItemDataLoaded(FName ItemID, UItemDataAsset* LoadedItem, FName CallbackFunctionName, UObject* CallbackTarget);
};