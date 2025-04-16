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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* RootComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* JumpTrigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* JumpPadMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* DestinationLocation;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "JumpPad")
    AJumpPadActor* DestinationJumpPad;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_JumpPadActive, Category = "JumpPad")
    bool bIsJumpPadActive;

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


    UFUNCTION(BlueprintCallable, Category = "JumpPad")
    bool IsTransporting() const;

    UPROPERTY()
    float AnimationStartTime;

    UFUNCTION()
    void OnRep_IsAnimating();

private:

    FVector OriginalJumpPadLocation;

    UPROPERTY(ReplicatedUsing = OnRep_IsAnimating)
    bool bIsAnimating;

    UPROPERTY()
    ACharacter* CurrentTransportingCharacter;

    FTimerHandle AnimationUpdateTimerHandle;


    bool IsDestinationClear(ACharacter* Character);
    bool ExecuteJumpAndTransport(ACharacter* Character);
    void StartCharacterTransport(ACharacter* Character);
    void TransportCharacter(float DeltaTime);
    void CompleteTransport();
    void OrientCharacterAfterTransport(ACharacter* Character, const FTransform& DestinationTransform);


    void UpdateJumpPadAnimation();
    void CompleteJumpPadAnimation();

    UFUNCTION()
    void OnJumpPadBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnJumpPadEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_LaunchCharacter(ACharacter* Character);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayJumpPadAnimation();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnJumpEffect(ACharacter* Character);

};
