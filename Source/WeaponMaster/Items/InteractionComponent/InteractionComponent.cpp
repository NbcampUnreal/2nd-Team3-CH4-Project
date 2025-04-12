// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

UInteractionComponent::UInteractionComponent(): InteractionWidget(nullptr), CurrentInteractor(nullptr)
{
    PrimaryComponentTick.bCanEverTick = false; // 틱 비활성화
    InteractionPrompt = FText::FromString(TEXT("F를 눌러 장착"));
}

void UInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInteractionComponent::SetInteractionPrompt(const FText& NewPrompt)
{
    InteractionPrompt = NewPrompt;
    
    // 이미 위젯이 표시되어 있다면 텍스트 업데이트
    if (InteractionWidget)
    {
        UTextBlock* PromptText = Cast<UTextBlock>(InteractionWidget->GetWidgetFromName(TEXT("PromptText")));
        if (PromptText)
        {
            PromptText->SetText(InteractionPrompt);
        }
    }
}

void UInteractionComponent::Interact(AActor* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("[%s] %s의 상호작용 요청"), 
        GetOwner() && GetOwner()->HasAuthority() ? TEXT("서버") : TEXT("클라이언트"),
        *Interactor->GetName());
    
    // 상호작용 이벤트 발생
    OnInteract.Broadcast(Interactor);
}

void UInteractionComponent::EnableInteraction(AActor* Interactor)
{
    if (Interactor)
    {
        CurrentInteractor = Interactor;
        ShowInteractionWidget(Interactor);
    }
}

void UInteractionComponent::DisableInteraction()
{
    HideInteractionWidget();
    CurrentInteractor = nullptr;
}

void UInteractionComponent::ShowInteractionWidget(AActor* Interactor)
{
    // 이미 위젯이 표시되어 있다면 리턴
    if (InteractionWidget)
    {
        return;
    }
    
    // 위젯 생성
    if (InteractionWidgetClass)
    {
        // 상호작용하는 액터의 플레이어 컨트롤러 얻기
        APawn* InteractorPawn = Cast<APawn>(Interactor);
        APlayerController* PC = nullptr;

        if (InteractorPawn && InteractorPawn->IsLocallyControlled())
        {
            // 로컬 플레이어인 경우에만 위젯 표시
            PC = Cast<APlayerController>(InteractorPawn->GetController());
            
            if (PC)
            {
                InteractionWidget = CreateWidget<UUserWidget>(PC, InteractionWidgetClass);
                if (InteractionWidget)
                {
                    // 위젯 표시
                    InteractionWidget->AddToViewport();
                    
                    // 프롬프트 텍스트 설정
                    UTextBlock* PromptText = Cast<UTextBlock>(InteractionWidget->GetWidgetFromName(TEXT("PromptText")));
                    if (PromptText)
                    {
                        PromptText->SetText(InteractionPrompt);
                    }
                    
                    // 소유자 위치에 위젯 위치 설정 (월드 -> 스크린 좌표)
                    FVector2D ScreenLocation;
                    if (PC->ProjectWorldLocationToScreen(
                        GetOwner()->GetActorLocation() + FVector(0, 0, 100), // 오브젝트 위에 위치
                        ScreenLocation))
                    {
                        InteractionWidget->SetPositionInViewport(ScreenLocation);
                    }
                }
            }
        }
    }
}

void UInteractionComponent::HideInteractionWidget()
{
    if (InteractionWidget)
    {
        InteractionWidget->RemoveFromParent();
        InteractionWidget = nullptr;
    }
}