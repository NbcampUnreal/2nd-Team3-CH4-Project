#include "InteractionComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Components/TextBlock.h"

UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    InteractionWidgetClass = nullptr;
    InteractionWidget = nullptr;
    CurrentInteractor = nullptr;
    InteractionPrompt = FText::FromString(TEXT("상호작용"));
}

void UInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInteractionComponent::SetInteractionPrompt(const FText& NewPrompt)
{
    // 텍스트 저장만 하고 위젯 업데이트는 하지 않음
    InteractionPrompt = NewPrompt;
    // 텍스트 설정 로직 제거
}

void UInteractionComponent::UpdatePromptText()
{
    if (InteractionWidget)
    {
        UTextBlock* TextLabel = Cast<UTextBlock>(InteractionWidget->GetWidgetFromName(TEXT("TextLabel")));
        if (TextLabel)
        {
            TextLabel->SetText(InteractionPrompt);
        }
    }
}

void UInteractionComponent::Interact(AActor* Interactor)
{
    // 상호작용 이벤트 발생 전 인자 검증
    if (Interactor && OnInteract.IsBound())
    {
        OnInteract.Broadcast(Interactor);
    }
}

void UInteractionComponent::EnableInteraction(AActor* Interactor)
{
    // 기존 위젯이 있으면 제거 (안전)
    DisableInteraction();
    
    if (!Interactor)
    {
        return;
    }
    
    // 인터랙터 참조 저장
    CurrentInteractor = Interactor;
    
    // 위젯 클래스 확인
    if (!InteractionWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("InteractionWidgetClass is not set!"));
        return;
    }
    
    // 상호작용자가 로컬 플레이어인지 확인
    APawn* InteractorPawn = Cast<APawn>(CurrentInteractor);
    if (!InteractorPawn || !InteractorPawn->IsLocallyControlled())
    {
        return;
    }
    
    APlayerController* PC = Cast<APlayerController>(InteractorPawn->GetController());
    if (!PC)
    {
        return;
    }
    
    // 위젯 생성 - 텍스트 설정 없이!
    InteractionWidget = CreateWidget<UUserWidget>(PC, InteractionWidgetClass);
    if (InteractionWidget)
    {
        // 텍스트 설정 로직 제거 - 위젯만 화면에 추가
        InteractionWidget->AddToViewport();
        
        // 화면상 위치 설정
        FVector2D ScreenLocation;
        if (PC->ProjectWorldLocationToScreen(
            GetOwner()->GetActorLocation() + FVector(0, 0, 100),
            ScreenLocation))
        {
            InteractionWidget->SetPositionInViewport(ScreenLocation);
        }
    }
}

void UInteractionComponent::DisableInteraction()
{
    // 위젯 제거
    if (InteractionWidget)
    {
        // 위젯과 관련된 참조 정리
        if (InteractionWidget->IsInViewport())
        {
            InteractionWidget->RemoveFromParent();
        }
        
        // 참조 정리
        InteractionWidget = nullptr;
    }
    
    // 상호작용자 참조 정리
    CurrentInteractor = nullptr;
}

void UInteractionComponent::CreateInteractionWidget()
{
    // 위젯 클래스 확인
    if (!InteractionWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("InteractionWidgetClass is not set!"));
        return;
    }
    
    // 상호작용자 확인
    APawn* InteractorPawn = Cast<APawn>(CurrentInteractor);
    if (!InteractorPawn || !InteractorPawn->IsLocallyControlled())
    {
        return;
    }
    
    // 플레이어 컨트롤러 확인
    APlayerController* PC = Cast<APlayerController>(InteractorPawn->GetController());
    if (!PC)
    {
        return;
    }
    
    // 위젯 생성
    InteractionWidget = CreateWidget<UUserWidget>(PC, InteractionWidgetClass);
    if (!InteractionWidget)
    {
        return;
    }
    
    // 텍스트 설정 후 화면에 추가
    UpdatePromptText();
    InteractionWidget->AddToViewport();
    
    // 화면상 위치 설정
    UpdateWidgetPosition();
}

void UInteractionComponent::UpdateWidgetPosition()
{
    if (!InteractionWidget || !GetOwner() || !CurrentInteractor)
    {
        return;
    }
    
    // 컨트롤러 얻기
    APawn* InteractorPawn = Cast<APawn>(CurrentInteractor);
    if (!InteractorPawn)
    {
        return;
    }
    
    APlayerController* PC = Cast<APlayerController>(InteractorPawn->GetController());
    if (!PC)
    {
        return;
    }
    
    // 화면 좌표 계산
    FVector2D ScreenLocation;
    bool bProjectSuccess = PC->ProjectWorldLocationToScreen(
        GetOwner()->GetActorLocation() + FVector(0, 0, 100),
        ScreenLocation
    );
    
    if (bProjectSuccess)
    {
        InteractionWidget->SetPositionInViewport(ScreenLocation);
    }
}