// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CombatState.h"
#include "Components/TextBlock.h"

void UCombatState::SetStateAlerting()
{
	if (IndicatorText)
	{
		IndicatorText->SetText(FText::FromString("?"));
	}
}

void UCombatState::SetStateCombat()
{
	if (IndicatorText)
	{
		IndicatorText->SetText(FText::FromString("!"));
	}
}
