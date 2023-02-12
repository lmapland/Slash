// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/StaminaPotion.h"

AStaminaPotion::AStaminaPotion()
{
	ItemName = "StaminaPotion";
	Amount = 25;
}

void AStaminaPotion::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);

	if (PickupInterface)
	{
		PickupInterface->AddStamina(this);
	}
}
