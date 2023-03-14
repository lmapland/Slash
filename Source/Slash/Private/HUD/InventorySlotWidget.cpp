// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InventorySlotWidget.h"
#include "Components/Inventory.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/SlashCharacter.h"
#include "HUD/SlashOverlay.h"
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

void UInventorySlotWidget::CtrlClicked(int32 SlotIndex, UInventory* Inventory)
{
	// Setup
	ACharacter* PC = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	ASlashCharacter* Character = Cast<ASlashCharacter>(PC);
	if (!Character) return;
	if (!Overlay)
	{
		Overlay = Character->GetSlashOverlay();
	}
	FInventorySlot* InventorySlot = Inventory->GetSlotRef(SlotIndex);
	
	if (!Overlay->CtrlPressed()) return;

	if (Overlay->ContainerIsOpen())
	{
		// Get the other inventory
		UInventory* OtherInventory;
		if (Inventory->IsOwnedByPlayer())
		{
			OtherInventory = Overlay->GetContainerInventory();
		}
		else
		{
			OtherInventory = Overlay->GetUserInventory();
		}

		// How should the items move?
		int32 OriginalAmount, NewAmount;
		OriginalAmount = NewAmount = InventorySlot->CurrentStack;
		OtherInventory->AddLenient(InventorySlot->ItemID, NewAmount);

		if (NewAmount == OriginalAmount) return; // nothing to do

		if (NewAmount == 0) // entire stack was moved
		{
			Inventory->Empty(SlotIndex);
			SetSlotEmpty();
		}
		else // split the stack
		{
			Inventory->RemoveFrom(SlotIndex, OriginalAmount - NewAmount);
		}

		Overlay->RefreshContainer();
		Overlay->RefreshUserInventory();
	}
	else
	{
		Character->DropItem(SlotIndex, true);
		SetSlotEmpty();
	}
}
