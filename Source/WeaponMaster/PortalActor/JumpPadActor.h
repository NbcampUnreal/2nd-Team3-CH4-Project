// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JumpPadActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class ACharacter;
class USoundBase;
class UParticleSystem;

UCLASS()
class WEAPONMASTER_API AJumpPadActor : public AActor
{
	GENERATED_BODY()
public:
    AJumpPadActor();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:


    // 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* RootComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* JumpTrigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* JumpPadMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* DestinationLocation;

    // 속성
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "JumpPad")
    AJumpPadActor* DestinationJumpPad;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_JumpPadActive, Category = "JumpPad")
    bool bIsJumpPadActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "JumpPad")
    float CooldownTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad")
    float JumpForce;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad")
    float TransportSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad")
    float DestinationOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad|Animation")
    float AnimationDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad|Animation")
    float JumpPadAnimHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad|Effects")
    USoundBase* JumpSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad|Effects")
    UParticleSystem* JumpEffectParticle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad|Materials")
    UMaterialInterface* ActiveMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad|Materials")
    UMaterialInterface* InactiveMaterial;

    // 함수
    UFUNCTION(BlueprintCallable, Category = "JumpPad")
    void SetJumpPadActive(bool bActive);

    UFUNCTION()
    void OnRep_JumpPadActive();

    UFUNCTION(BlueprintCallable, Category = "JumpPad")
    void UpdateJumpPadMaterial();

    UPROPERTY()
    FTimerHandle TransportTimerHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad")
    float TransportUpdateInterval;

    void StartTransportTimer(ACharacter* Character);
    void StopTransportTimer();
    UFUNCTION()
    void OnTransportTimerTick();

    // 이미 전송 중인지 확인하는 메서드 추가
    UFUNCTION(BlueprintCallable, Category = "JumpPad")
    bool IsTransporting() const;

    UPROPERTY()
    float AnimationStartTime;

    UFUNCTION()
    void OnRep_IsAnimating();

private:
    // 원래 위치 (애니메이션용)
    FVector OriginalJumpPadLocation;

    UPROPERTY(ReplicatedUsing = OnRep_IsAnimating)
    bool bIsAnimating;

    

    // 현재 이동 중인 캐릭터
    UPROPERTY()
    ACharacter* CurrentTransportingCharacter;


    // 타이머 핸들
    FTimerHandle CooldownTimerHandle;
    FTimerHandle AnimationUpdateTimerHandle;
    FTimerHandle AnimationCompleteTimerHandle;


    bool IsDestinationClear(ACharacter* Character);
    bool ExecuteJumpAndTransport(ACharacter* Character);
    void StartCharacterTransport(ACharacter* Character);
    void TransportCharacter(float DeltaTime);
    void CompleteTransport();
    void OnCooldownComplete();
    void OrientCharacterAfterTransport(ACharacter* Character, const FTransform& DestinationTransform);


    void UpdateJumpPadAnimation();
    void CompleteJumpPadAnimation();

    // 오버랩 이벤트 핸들러
    UFUNCTION()
    void OnJumpPadBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnJumpPadEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // 네트워크 RPC
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_LaunchCharacter(ACharacter* Character);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayJumpPadAnimation();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnJumpEffect(ACharacter* Character);

};
