// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	void RegenStamina(float DeltaTime);
	void DrainStamina(float DeltaTime);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;
	
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaDrainRate = 12.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Level = 1;

	TArray<int32> SoulsPerLevel = { 0, 3, 5, 8, 12, 17, 23, 30, 38, 47 };

public:
	void ReceiveDamage(float Damage);
	bool UseStaminaIfAble(int32 Value);
	float GetHealthPercent();
	float GetStaminaPercent();
	bool IsAlive();
	void AddSouls(int32 Value);
	void AddGold(int32 Value);
	void AddFlowers(int32 Value);
	void AddOre(int32 Value);
	void LevelUp();
	float AddAttribute(int32 ItemID, int32 Amount);

	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE int32 GetLevel() const { return Level; }
	FORCEINLINE int32 GetSoulsUntilNextLevel() const { return SoulsPerLevel[Level]; }
	FORCEINLINE float GetPercentToNextLevel() const { return (Souls * 1.f) / (SoulsPerLevel[Level] * 1.f); }

	FORCEINLINE void AddHealth(int32 Value) { Health = FMath::Clamp(Health + Value, 0.f, MaxHealth); }
	FORCEINLINE void AddStamina(int32 Value) { Stamina = FMath::Clamp(Stamina + Value, 0.f, MaxStamina); }
		
};
