#include "SessionLobbyWidget.h"

#include "Components/Button.h"

void USessionLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartSessionButton)
	{
		StartSessionButton->OnClicked.AddDynamic(this, &USessionLobbyWidget::HandleStartSessionButtonClicked);
	}
}

void USessionLobbyWidget::HandleStartSessionButtonClicked()
{
	OnStartSessionClicked.Broadcast();
}