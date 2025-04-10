#include "CharacterBoxWidget.h"

UCharacterBoxWidget::UCharacterBoxWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 생성자에서 특별한 초기화 필요 없음
}

void UCharacterBoxWidget::SetCharacterClass(TSubclassOf<ACharacter> InCharacterClass)
{
	// 데이터 저장
	CharacterClass = InCharacterClass;
    
	// 이름 표시 업데이트
	UpdateDisplayName();
}

void UCharacterBoxWidget::UpdateDisplayName()
{
	if (!TextBlock || !CharacterClass)
	{
		return;
	}
    
	// 캐릭터 클래스 이름 표시
	FString ClassName = CharacterClass->GetName();
	// BP_ 또는 다른 접두사 제거 (선택적)
	ClassName.ReplaceInline(TEXT("BP_"), TEXT(""));
	ClassName.ReplaceInline(TEXT("Character"), TEXT(""));
    
	TextBlock->SetText(FText::FromString(ClassName));
    
	// 디버깅용 로그
	UE_LOG(LogTemp, Log, TEXT("CharacterBoxWidget - 이름 업데이트: %s"), *ClassName);
}

void UCharacterBoxWidget::OnItemClicked()
{
	// 로그에 선택된 캐릭터 클래스 출력
	if (CharacterClass)
	{
		FString ClassName = CharacterClass->GetName();
		UE_LOG(LogTemp, Log, TEXT("CharacterBoxWidget - 선택된 캐릭터 클래스: %s"), *ClassName);
	}
    
	// 델리게이트 브로드캐스트
	OnCharacterClassClicked.Broadcast(CharacterClass);
}

FReply UCharacterBoxWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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