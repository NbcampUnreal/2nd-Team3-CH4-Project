#include "OptionMenuWidget.h"
#include "OptionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UOptionMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 기존 버튼 이벤트 바인딩
    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddDynamic(this, &UOptionMenuWidget::OnResumeClicked);
    }
    if (MainMenuButton)
    {
        MainMenuButton->OnClicked.AddDynamic(this, &UOptionMenuWidget::OnMainMenuClicked);
    }
    if (OptionButton)
    {
        OptionButton->OnClicked.AddDynamic(this, &UOptionMenuWidget::OnOptionClicked);
    }


}

void UOptionMenuWidget::OnResumeClicked()
{
    RemoveFromParent();
}
void UOptionMenuWidget::OnMainMenuClicked()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName("Test_Bong"));
}
void UOptionMenuWidget::OnOptionClicked()
{
    // 이미 생성된 오디오 위젯이 있다면 중복 생성 방지
    
    if (OptionWidget)
    {
        OptionWidget->RemoveFromParent();
        OptionWidget = nullptr;
    }

    SetVisibility(ESlateVisibility::Hidden);

    if (OptionWidgetClass)
    {
        OptionWidget = CreateWidget<UOptionWidget>(GetOwningPlayer(), OptionWidgetClass);
        if (OptionWidget)
        {
            OptionWidget->OptionComplete.AddDynamic(this, &UOptionMenuWidget::OnOptionCompleted);
            OptionWidget->AddToViewport();
        }
    }
}
void UOptionMenuWidget::OnOptionCompleted()
{
    SetVisibility(ESlateVisibility::Visible);
    OptionWidget->RemoveFromParent();
    OptionWidget = nullptr;
}