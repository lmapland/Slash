// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetGold(int32 Gold)
{
	if (GoldText)
	{
		GoldText->SetText(FText::FromString(FString::Printf(TEXT("%i"), Gold)));
	}
}

void USlashOverlay::SetSouls(int32 Souls)
{
	if (SoulsText)
	{
		SoulsText->SetText(FText::FromString(FString::Printf(TEXT("%i"), Souls)));
	}
}

void USlashOverlay::SetFlowers(int32 Flowers)
{
	if (FlowersText)
	{
		FlowersText->SetText(FText::FromString(FString::Printf(TEXT("%i"), Flowers)));
	}

}

void USlashOverlay::SetOre(int32 Ore)
{
	if (OreText)
	{
		OreText->SetText(FText::FromString(FString::Printf(TEXT("%i"), Ore)));
	}
}
