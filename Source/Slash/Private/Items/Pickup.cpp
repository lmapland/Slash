// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Pickup.h"


void APickup::PickedUp()
{
	Destroy();
}

void APickup::Pick()
{
	bPickable = false;
}