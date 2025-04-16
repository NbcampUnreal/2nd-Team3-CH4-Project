#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

class FBaseBufferedInput
{
protected:
	bool bIsValidInput = true;
	FTimerHandle ValidTimerHandle;
	TObjectPtr<ACharacter> OwnerCharacter = nullptr;
	
public:
	virtual ~FBaseBufferedInput() = default;

	virtual void Initialize(ACharacter* InitialOwnerCharacter)
	{
		OwnerCharacter = InitialOwnerCharacter;
		if (IsValid(OwnerCharacter))
		{
			OwnerCharacter->GetWorldTimerManager().SetTimer(
			ValidTimerHandle,
			FTimerDelegate::CreateLambda([this]()
			{
				bIsValidInput = false;
			}),
			0.3f,
			false
			);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BaseBufferedInput::Initialize : InitialCharacterSetting Failed."))
		}
	}
	
	virtual void Operate() {}

	bool IsValidInput() const { return bIsValidInput; }
};
