// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupableItem.generated.h"

class UInteractionComponent;
class UItemDataAsset;

UCLASS()
class WEAPONMASTER_API APickupableItem : public AActor
{
	GENERATED_BODY()
    
public:    
	APickupableItem();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// 아이템 ID
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID;
    
	// 아이템 데이터
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Item")
	UItemDataAsset* ItemData;
    
	// 아이템 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;
    
	// 오버랩 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* InteractionSphere;
    
	// 상호작용 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInteractionComponent* InteractionComponent;
    
	// 아이템 획득 처리
	UFUNCTION()
	void OnPickup(AActor* Interactor);

	// 아이템 데이터 로드
	UFUNCTION()
	void LoadItemData();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void ProcessPickup(AActor* Interactor);
	
	UFUNCTION(Server, Reliable)
	void Server_OnPickedUp(AActor* Interactor);

	UFUNCTION(Client, Reliable)
	void Client_OnPickupSuccess();
protected:
    
	// 오버랩 시작 이벤트
	UFUNCTION()
	void OnInteractionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
						   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						   bool bFromSweep, const FHitResult& SweepResult);
    
	// 오버랩 종료 이벤트
	UFUNCTION()
	void OnInteractionEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
						 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
