// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Instant.h"
#include "StaminaPotion.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AStaminaPotion : public AInstant
{
	GENERATED_BODY()

public:
	AStaminaPotion();

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
