// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CombatStateComponent.h"
#include "HUD/CombatState.h"

void UCombatStateComponent::SetState(int32 State)
{
	if (CombatStateWidget == nullptr)
	{
		CombatStateWidget = Cast<UCombatState>(GetUserWidgetObject());
	}

	switch (State)
	{
	case 0:
		SetVisibility(false);
		break;
	case 1:
		CombatStateWidget->SetStateAlerting();
		SetVisibility(true);
		break;
	case 2:
		CombatStateWidget->SetStateCombat();
		SetVisibility(true);
		break;
	}
}
