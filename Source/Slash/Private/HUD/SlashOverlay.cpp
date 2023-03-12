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

void USlashOverlay::SetLevelInfo(int32 Level, float LevelPercent)
{
	if (LevelText)
	{
		LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level %i"), Level)));
	}

	if (LevelProgressBar)
	{
		LevelProgressBar->SetPercent(LevelPercent);
	}
}

void USlashOverlay::OpenEverythingMenu(int32 Souls, int32 SoulsNeeded, int32 NextLevel)
{
	ShowTabWidget(Souls, SoulsNeeded, NextLevel);
}

void USlashOverlay::UpdateAttribute(int32 ItemID, float Value)
{
	switch (ItemID)
	{
	case 0:
		SetHealthBarPercent(Value);
		break;
	case 1:
		SetStaminaBarPercent(Value);
		break;
	case 2:
		SetSouls((int32)Value);
		break;
	case 3:
		SetGold((int32)Value);
		break;
	}
}
