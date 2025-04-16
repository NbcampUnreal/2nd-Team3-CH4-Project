// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DebuffWidgetUser.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDebuffWidgetUser : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WEAPONMASTER_API IDebuffWidgetUser
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void UpdateDebuffWidget() = 0;
};
