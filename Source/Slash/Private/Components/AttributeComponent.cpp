// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

void UAttributeComponent::DrainStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina - StaminaDrainRate * DeltaTime, 0.f, MaxStamina);
}

// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

bool UAttributeComponent::UseStaminaIfAble(int32 Value)
{
	if (Stamina >= Value)
	{
		Stamina -= Value;
		return true;
	}

	return false;
}

float UAttributeComponent::GetHealthPercent()
{
	return (Health / MaxHealth);
}

float UAttributeComponent::GetStaminaPercent()
{
	return (Stamina / MaxStamina);
}

bool UAttributeComponent::IsAlive()
{
	return (Health > 0);
}

void UAttributeComponent::AddSouls(int32 Value)
{
	Souls += Value;
}

void UAttributeComponent::AddGold(int32 Value)
{
	Gold += Value;
}

void UAttributeComponent::AddFlowers(int32 Value)
{
	Flowers += Value;
}

void UAttributeComponent::AddOre(int32 Value)
{
	Ore += Value;
}

void UAttributeComponent::LevelUp()
{
	// User has clicked the button. First we verify that the user has enough Souls to level, then do it
	if (Souls < SoulsPerLevel[Level])
	{
		UE_LOG(LogTemp, Warning, TEXT("In LevelUp(): Could not level up; user has %i out of %i Souls"), Souls, SoulsPerLevel[Level]);
		return;
	}

	Souls -= SoulsPerLevel[Level];
	Level += 1;
}
