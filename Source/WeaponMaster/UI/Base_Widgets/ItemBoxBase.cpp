#include "ItemBoxBase.h"

UItemBoxBase::UItemBoxBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 생성자에서는 초기화만 수행
	// 위젯을 상호작용 가능하게 설정
	SetIsFocusable(true);
}

void UItemBoxBase::NativePreConstruct()
{
	Super::NativePreConstruct();
    
	// 디자이너에서 변경된 값을 적용
	if (SizeBox)
	{
		SizeBox->SetWidthOverride(BoxWidth);
		SizeBox->SetHeightOverride(BoxHeight);
	}
}

void UItemBoxBase::NativeConstruct()
{
	Super::NativeConstruct();
    
	// Border의 초기 설정
	if (Border)
	{
		Border->SetHorizontalAlignment(HAlign_Center);
		Border->SetVerticalAlignment(VAlign_Bottom);
		Border->SetBrushColor(NormalColor);
	}
}

void UItemBoxBase::SetItemBoxSize(float Width, float Height)
{
	BoxWidth = Width;
	BoxHeight = Height;
    
	if (SizeBox)
	{
		SizeBox->SetWidthOverride(Width);
		SizeBox->SetHeightOverride(Height);
	}
}

FReply UItemBoxBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 자식 클래스에서 이벤트 처리 로직 구현
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UItemBoxBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    
	// 호버 시 Border 색상 변경
	if (Border)
	{
		Border->SetBrushColor(HoverColor);
	}
}

void UItemBoxBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
    
	// 호버 해제 시 Border 색상 원래대로
	if (Border)
	{
		Border->SetBrushColor(NormalColor);
	}
}