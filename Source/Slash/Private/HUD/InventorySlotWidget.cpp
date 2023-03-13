// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InventorySlotWidget.h"
#include "Components/Inventory.h"

void UInventorySlotWidget::MoveAcrossInventories(int32 SourceSlotIndex, int32 DestSlotIndex, UInventory* SourceInventory, UInventory* DestInventory)
{
	FInventorySlot* Source = SourceInventory->GetSlotRef(SourceSlotIndex);
	FInventorySlot* Dest = DestInventory->GetSlotRef(DestSlotIndex);

	// If the itemids are the same, then stack as high as possible. Add to one, subtract from the other, update both
	if (Dest->ItemID == Source->ItemID)
	{
		//UE_LOG(LogTemp, Warning, TEXT("In MoveAcrossInventories(): ItemIDs are the same; trying to stack"));
		int32 ToAdd = (Source->CurrentStack + Dest->CurrentStack > Dest->MaxStack) ? Dest->MaxStack - Dest->CurrentStack : Source->CurrentStack;
		
		Dest->CurrentStack += ToAdd;
		Source->CurrentStack -= ToAdd;

		DestInventory->UpdateSlot(Dest, DestSlotIndex);

		if (Source->CurrentStack == 0)
		{
			Source->MaxStack = 0;
			Source->ItemID = -1;
		}
		SourceInventory->UpdateSlot(Source, SourceSlotIndex);
	}
	else // ItemIDs are different; do straight up swaps
	{
		//UE_LOG(LogTemp, Warning, TEXT("In MoveAcrossInventories(): ItemIDs are not the same; swapping"));
		DestInventory->UpdateSlot(Source, DestSlotIndex);
		SourceInventory->UpdateSlot(Dest, SourceSlotIndex);
	}
}