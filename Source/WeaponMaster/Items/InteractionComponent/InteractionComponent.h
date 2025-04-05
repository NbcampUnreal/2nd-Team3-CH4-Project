// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractSignature, class AActor*, Interactor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WEAPONMASTER_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
    
public:    
	UInteractionComponent();
    
	virtual void BeginPlay() override;
    
	// 상호작용 프롬프트 텍스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionPrompt;
    
	// 상호작용 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractSignature OnInteract;
    
	// 상호작용 프롬프트 설정
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionPrompt(const FText& NewPrompt);
    
	// 상호작용 함수
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);
    
	// 상호작용 활성화
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void EnableInteraction(AActor* Interactor);
    
	// 상호작용 비활성화
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void DisableInteraction();

protected:
	// 상호작용 UI 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TSubclassOf<class UUserWidget> InteractionWidgetClass;
    
	// 현재 표시 중인 위젯
	UPROPERTY()
	UUserWidget* InteractionWidget;
    
	// 현재 상호작용 중인 액터
	UPROPERTY()
	AActor* CurrentInteractor;
    
	// 상호작용 위젯 표시
	void ShowInteractionWidget(AActor* Interactor);
    
	// 상호작용 위젯 숨기기
	void HideInteractionWidget();
};
