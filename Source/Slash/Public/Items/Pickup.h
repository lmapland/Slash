// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Pickup.generated.h"

/**
 * Items that need the character to bend over to pick up should be parented from here
 */
UCLASS()
class SLASH_API APickup : public AItem
{
	GENERATED_BODY()
public:
	void PickedUp(); // finalize the pickup (destroy this object)
	void Pick(); // Pick the object, causing it to not be pickable anymore

	float AnimSpeed = 1.f;

private:
	bool bPickable = true; // by default object can be picked
	int32 Amount = 1;

public:
	FORCEINLINE bool Pickable() const { return bPickable; }
	FORCEINLINE int32 GetAmount() const { return Amount; }

};
