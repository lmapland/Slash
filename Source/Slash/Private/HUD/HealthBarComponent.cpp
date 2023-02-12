// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	if (HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}

	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		if (Percent < 0.f) Percent = 0.f;
		if (Percent > 100.f) Percent = 100.f;

		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}