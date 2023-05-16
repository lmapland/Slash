// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "CombatStateComponent.generated.h"

class UCombatState;

/**
 * 
 */
UCLASS()
class SLASH_API UCombatStateComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void SetState(int32 State);

private:
	UPROPERTY()
	UCombatState* CombatStateWidget;

};
