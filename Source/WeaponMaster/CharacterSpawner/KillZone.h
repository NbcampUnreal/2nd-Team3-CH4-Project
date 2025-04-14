#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillZone.generated.h"

class UBoxComponent;

UCLASS()
class WEAPONMASTER_API AKillZone : public AActor
{
	GENERATED_BODY()

public:
	AKillZone();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
											UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
											bool bFromSweep, const FHitResult& SweepResult);

public:
	virtual void Tick(float DeltaTime) override;
};
