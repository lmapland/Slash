// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Merchant.h"
#include "HUD/SlashOverlay.h"

void AMerchant::Interact(USlashOverlay* Overlay, UAttributeComponent* Attributes)
{
	Overlay->ShowMerchantMenu(ItemsToSell, ItemsToBuy);
}
