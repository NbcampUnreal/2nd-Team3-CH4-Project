#include "SessionLobbyWidget.h"

#include "Components/Button.h"

void USessionLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartSessionButton)
	{
		StartSessionButton->OnClicked.AddDynamic(this, &USessionLobbyWidget::HandleStartSessionButtonClicked);
	}

	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &USessionLobbyWidget::HandleLoginButtonClicked);
	}
}

void USessionLobbyWidget::HandleStartSessionButtonClicked()
{
	OnStartSessionClicked.Broadcast();
}

void USessionLobbyWidget::HandleLoginButtonClicked()
{
	OnLoginClicked.Broadcast();
}
