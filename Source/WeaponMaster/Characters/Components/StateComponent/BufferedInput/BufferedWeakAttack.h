#pragma once

#include "BaseBufferedInput.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/StateComponent/StateComponent.h"

class FBufferedWeakAttack : public FBaseBufferedInput
{
public:
	virtual ~FBufferedWeakAttack() override = default;

	virtual void Initialize(ACharacter* InitialOwnerCharacter) override
	{
		FBaseBufferedInput::Initialize(InitialOwnerCharacter);
	}
	
	virtual void Operate() override
	{
		if (bIsValidInput && OwnerCharacter.IsValid())
		{
			if (OwnerCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
			{
				TScriptInterface<UBehaviorState> BehaviorState = IBattleSystemUser::Execute_GetBehaviorState(OwnerCharacter.Get());
				if (auto CastedInputState = Cast<IBehaviorState>(BehaviorState.GetObject()))
				{
					CastedInputState->WeakAttack();
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Buffered WeakAttack Error"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Buffered WeakAttack Expired"));
		}
	}
};
