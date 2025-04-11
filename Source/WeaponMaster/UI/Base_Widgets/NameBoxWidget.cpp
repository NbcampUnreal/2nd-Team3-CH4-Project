#include "NameBoxWidget.h"

UNameBoxWidget::UNameBoxWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 생성자에서 특별한 초기화 필요 없음
}

void UNameBoxWidget::SetObjectName(const FName& InObjectName)
{
	// 데이터 저장
	ObjectName = InObjectName;
    
	// 이름 표시 업데이트
	UpdateDisplayName();
}

void UNameBoxWidget::UpdateDisplayName()
{
	if (!TextBlock)
	{
		return;
	}
    
	// 아이템 이름 표시 (DA_ 접두사 제거)
	FString ItemName = ObjectName.ToString();
	// DA_ 접두사 제거
	ItemName.ReplaceInline(TEXT("DA_"), TEXT(""));
    
	TextBlock->SetText(FText::FromString(ItemName));
    
	// 디버깅용 로그
	UE_LOG(LogTemp, Log, TEXT("NameBoxWidget - 이름 업데이트: %s"), *ItemName);
}

void UNameBoxWidget::OnItemClicked()
{
	// 로그에 선택된 아이템 출력
	UE_LOG(LogTemp, Log, TEXT("NameBoxWidget - 선택된 아이템: %s"), *ObjectName.ToString());
    
	// 델리게이트 브로드캐스트 (위젯 자신도 전달)
	OnObjectNameClicked.Broadcast(ObjectName, this);
}

FReply UNameBoxWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 마우스 왼쪽 버튼 클릭 시
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		// 클릭 이벤트 처리
		OnItemClicked();
		return FReply::Handled();
	}
    
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}