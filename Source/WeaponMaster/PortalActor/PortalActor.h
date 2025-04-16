// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalActor.generated.h"

class UBoxComponent;

UCLASS()
class WEAPONMASTER_API APortalActor : public AActor
{
	GENERATED_BODY()

public:
    APortalActor();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

public:

    // 포탈 콜리전 박스 (텔레포트 감지용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TelePortal")
    UBoxComponent* PortalTrigger;

    // 텔레포트 될 위치를 나타내는 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TelePortal")
    USceneComponent* TeleportLocation;

    // 연결된 대상 포탈 (텔레포트할 포탈)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TelePortal", Replicated)
    APortalActor* DestinationPortal = nullptr;

    // 포탈 활성/비활성 상태
    UPROPERTY(ReplicatedUsing = OnRep_PortalActive, VisibleAnywhere, BlueprintReadOnly, Category = "TelePortal")
	bool bIsPortalActive = true;

    // 텔레포트 쿨다운 시간 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TelePortal", Replicated)
    float TeleportCooldown = 1.f;

    UFUNCTION()
    void OnRep_PortalActive();

    void UpdatePortalMaterial();

    UFUNCTION()
    void OnPortalBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPortalEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


    UFUNCTION(Server, Reliable, WithValidation)
    void Server_TeleportCharacter(ACharacter* Character);
    bool Server_TeleportCharacter_Validate(ACharacter* Character);


    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnTeleportEffect(ACharacter* Character);

    // 포탈 활성화/비활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Portal")
    void SetPortalActive(bool bActive);

    // 텔레포트 적합성 검사 함수
    UFUNCTION(BlueprintCallable, Category = "Portal")
    bool IsDestinationClear(ACharacter* Character);

    // 텔레포트 실행 함수
    UFUNCTION(BlueprintCallable, Category = "Portal")
    bool ExecuteTeleport(ACharacter* Character);


    // 텔레포트 쿨다운 타이머 핸들러
    FTimerHandle TeleportCooldownTimerHandle;

    // 텔레포트 후 캐릭터 방향 처리
    void OrientCharacterAfterTeleport(ACharacter* Character, const FTransform& DestinationTransform);

    // 텔레포트 쿨다운 완료 핸들러
    UFUNCTION()
    void OnTeleportCooldownComplete();
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	UStaticMeshComponent* PortalMesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    UStaticMeshComponent* PortalComponentMesh2 = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    UStaticMeshComponent* PortalComponentMesh3 = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    UStaticMeshComponent* PortalComponentMesh4 = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    UStaticMeshComponent* PortalComponentMesh5 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Portal")
    UMaterialInterface* ActiveMaterial = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Portal")
    UMaterialInterface* InactiveMaterial = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    UParticleSystem* TeleportEffectParticle = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    USoundBase* TeleportSound = nullptr;
};
