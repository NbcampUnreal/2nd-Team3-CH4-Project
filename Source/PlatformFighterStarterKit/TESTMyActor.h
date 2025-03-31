

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TESTMyActor.generated.h"

UCLASS()
class PLATFORMFIGHTERSTARTERKIT_API ATESTMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATESTMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
