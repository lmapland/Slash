// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Interfaces/PickupInterface.h"
#include "Instant.generated.h"

/**
 * Items that only need the character to overlap the item's sphere in order for the item to be "picked up" should be parented from here
 */
UCLASS()
class SLASH_API AInstant : public AItem
{
	GENERATED_BODY()
public:
	void Overlapped();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Properties | Sounds")
	class USoundBase* PickupSound;

	UPROPERTY(EditAnywhere, Category = "Properties")
	int32 Amount = 0;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:
	FORCEINLINE int32 GetAmount() const { return Amount; }

};
