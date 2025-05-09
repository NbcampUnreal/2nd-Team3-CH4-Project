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
	
	// Border_Outline 초기 설정 - 처음에는 숨김
	if (Border_Outline)
	{
	    Border_Outline->SetVisibility(ESlateVisibility::Hidden);
	}
	
	// 초기 선택 상태 설정
	bSelected = false;
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

void UItemBoxBase::SetBorderImage(UTexture2D* Image)
{
	// 내부 Border에만 이미지 설정
	if (!Border || !Image)
	{
		return;
	}
    
	// 이미지 브러시 생성 및 설정
	FSlateBrush Brush;
	Brush.SetResourceObject(Image);
	Brush.ImageSize = FVector2D(Image->GetSizeX(), Image->GetSizeY());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	
	// 내부 보더에 브러시 설정
	Border->SetBrushFromTexture(Image);
	
	// 디버깅용 로그
	UE_LOG(LogTemp, Log, TEXT("ItemBoxBase - 아이템 이미지 설정: %s"), *Image->GetName());
}

void UItemBoxBase::SetBorderColor(const FLinearColor& Color)
{
	if (Border)
	{
		Border->SetBrushColor(Color);
	}
}

void UItemBoxBase::SetSelected(bool bInSelected)
{
    bSelected = bInSelected;
    
    // 선택 상태에 따라 테두리 보더 가시성 설정
    if (Border_Outline)
    {
        Border_Outline->SetVisibility(bSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
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