#pragma once

#include "BaseBufferedInput.h"
#include "Characters/Components/IBattleSystemUser.h"
#include "Characters/Components/StateComponent/StateComponent.h"

class FBufferedStrongAttack : public FBaseBufferedInput
{
public:
	virtual ~FBufferedStrongAttack() override = default;

	virtual void Initialize(ACharacter* InitialOwnerCharacter) override
	{
		FBaseBufferedInput::Initialize(InitialOwnerCharacter);
	}
	
	virtual void Operate() override
	{
		if (bIsValidInput)
		{
			if (OwnerCharacter->GetClass()->ImplementsInterface(UBattleSystemUser::StaticClass()))
			{
				TScriptInterface<UBehaviorState> BehaviorState = IBattleSystemUser::Execute_GetBehaviorState(OwnerCharacter);
				UE_LOG(LogTemp, Display, TEXT("Buffered StrongAttack Operate"));
			}
		}
	}
};
