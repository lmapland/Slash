// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"
#include "Subsystems/EventsSubsystem.h"
#include "Components/QuestObjectiveTypes.h"

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

bool UAttributeComponent::Contains(int32 ItemID, int32 Amount)
{
	switch (ItemID)
	{
	case 0:
		return Health >= Amount? true : false;
		break;
	case 1:
		return Stamina >= Amount ? true : false;
		break;
	case 2:
		return Souls >= Amount ? true : false;
		break;
	case 3:
		return Gold >= Amount ? true : false;
		break;
	default:
		return false;
		break;
	}
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	EventsSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UEventsSubsystem>();

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

bool UAttributeComponent::LevelUp()
{
	// User has clicked the button. First we verify that the user has enough Souls to level, then do it
	if (Souls < SoulsPerLevel[Level])
	{
		UE_LOG(LogTemp, Warning, TEXT("In LevelUp(): Could not level up; user has %i out of %i Souls"), Souls, SoulsPerLevel[Level]);
		return false;
	}

	Souls -= SoulsPerLevel[Level];
	Level += 1;

	return true;
}

float UAttributeComponent::AddAttribute(int32 ItemID, int32 Amount)
{
	switch (ItemID)
	{
	case 0:
		AddHealth(Amount);
		if (EventsSubsystem) EventsSubsystem->CreateEvent(EObjectiveType::EOT_AttributeUpdated, ItemID, Health);
		return Health / MaxHealth;
		break;
	case 1:
		AddStamina(Amount);
		if (EventsSubsystem) EventsSubsystem->CreateEvent(EObjectiveType::EOT_AttributeUpdated, ItemID, Stamina);
		return Stamina / MaxStamina;
		break;
	case 2:
		AddSouls(Amount);
		if (EventsSubsystem) EventsSubsystem->CreateEvent(EObjectiveType::EOT_AttributeUpdated, ItemID, Souls);
		return (float)Souls;
		break;
	case 3:
		AddGold(Amount);
		if (EventsSubsystem) EventsSubsystem->CreateEvent(EObjectiveType::EOT_AttributeUpdated, ItemID, Gold);
		return (float)Gold;
		break;
	default:
		return 0;
		break;
	}
}
