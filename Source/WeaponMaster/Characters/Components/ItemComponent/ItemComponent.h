﻿#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemComponent.generated.h"

class APickupableItem;
struct FStreamableHandle;
class UItemDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemEquipped, UItemDataAsset*, EquippedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemUnequipped);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WEAPONMASTER_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UItemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/** 아이템 장착 */
    UFUNCTION(BlueprintCallable, Category = "Items")
    bool EquipItem(FName ItemID);
    
    /** 아이템 장착 해제 */
    UFUNCTION(BlueprintCallable, Category = "Items")
    void UnequipItem();
    
    /** 현재 장착된 아이템 반환 */
    UFUNCTION(BlueprintPure, Category = "Items")
    UItemDataAsset* GetEquippedItem() const;
	
    /** 아이템 장착 완료 이벤트 */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnItemEquipped OnItemEquipped;
    
    /** 아이템 장착 해제 이벤트 */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnItemUnequipped OnItemUnequipped;
    
    /** 무기 충돌 활성화/비활성화 */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetWeaponCollisionEnabled(bool bEnabled);

	/** 스폰할 픽업 아이템 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<APickupableItem> PickupItemClass;
protected:
    /** 현재 장착된 아이템 */
    UPROPERTY(Replicated,VisibleAnywhere, Category = "Items")
    UItemDataAsset* EquippedItem;
	
    /** 아이템 비동기 로딩 */
    void AsyncLoadItem(FName ItemID);
    
    /** 로딩 완료 콜백 */
    void OnItemLoaded(FName ItemID);
    
    /** 아이템을 소켓에 부착 */
    void AttachItemToSocket(UItemDataAsset* Item);
    
    /** 아이템 시각 효과 처리 */
    void HandleItemVisualEffects(UItemDataAsset* Item, bool bEquipping);
    
    /** 메시 비동기 로드 완료 핸들러 */
    void OnMeshLoadCompleted();

	/** 아이템 픽업 스폰 함수 */
	UFUNCTION()
	void SpawnPickupItem(UItemDataAsset* ItemData);

	/** 서버에 아이템 장착 해제 요청 */
	UFUNCTION(Server, Reliable)
	void Server_UnequipItem(FName ItemID);
private:
    /** 비동기 로드 요청 핸들 */
    TSharedPtr<FStreamableHandle> LoadHandle;
    
    /** 무기 충돌 처리 */
	UFUNCTION()
    void HandleWeaponCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    
    /** 서버에 아이템 장착 요청 */
    UFUNCTION(Server, Reliable)
    void Server_EquipItem(FName ItemID);
    
    /** 클라이언트에 아이템 장착 결과 알림 */
    UFUNCTION(Client, Reliable)
    void Client_OnItemEquipped(UItemDataAsset* NewItem);
};