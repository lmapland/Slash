// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Inventory.h"

UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Should be used for PC inventories only
bool UInventory::AddStrict(int32 ItemID, int32 Amount)
{
	FItemStructure* Row = TableOfItems->FindRow<FItemStructure>(FName(FString::FromInt(ItemID)), Context);

	if (!HasSpace(ItemID, Amount, Row->max_stack))
	{
		UE_LOG(LogTemp, Warning, TEXT("AddStrict(): There isn't enough space for %i of itemID %i "), Amount, ItemID);
		return false;
	}

	// Add items
	int32 LeftToAdd = Amount;
	int32 MaxStack = Row->max_stack;

	// First add to any stacks that already exist
	AddToStack(ItemID, LeftToAdd);
	if (LeftToAdd <= 0) return true;

	// either no stacks exist or the amount is bigger than the current stack can hold
	if (Row->usable) { AddFromTopDown(ItemID, LeftToAdd, MaxStack); }
	else { AddFromBottomUp(ItemID, LeftToAdd, MaxStack); }

	UE_LOG(LogTemp, Warning, TEXT("AddStrict(): Done adding."));
	return true;
}

TSubclassOf<AItem> UInventory::AddLenient(int32 ItemID, int32& Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("AddLenient(): ItemID: %i, Amount: %i"), ItemID, Amount);
	FItemStructure* Row = TableOfItems->FindRow<FItemStructure>(FName(FString::FromInt(ItemID)), Context);

	// Add items
	int32 MaxStack = Row->max_stack;

	// First add to any stacks that already exist
	AddToStack(ItemID, Amount);
	if (Amount <= 0) return nullptr;

	if (OwnedByUser)
	{
		// either no stacks exist or the amount is bigger than the current stack can hold
		if (Row->usable) { AddFromTopDown(ItemID, Amount, MaxStack); }
		else { AddFromBottomUp(ItemID, Amount, MaxStack); }
	}
	else
	{
		AddFromTopDown(ItemID, Amount, MaxStack);
	}

	//UE_LOG(LogTemp, Warning, TEXT("AddLenient(): Done adding, %i left."), Amount);
	return Row->class_ref;
}

bool UInventory::AddToSlot(int32 SlotIndex, int32 ItemID, int32 Amount)
{
	FItemStructure* Row = TableOfItems->FindRow<FItemStructure>(FName(FString::FromInt(ItemID)), Context);

	if (Slots[SlotIndex]->ItemID != -1) return false;
	if (Amount > Row->max_stack) return false;
	
	Slots[SlotIndex]->ItemID = ItemID;
	Slots[SlotIndex]->CurrentStack = Amount;
	Slots[SlotIndex]->MaxStack = Row->max_stack;

	return true;
}

FInventorySlot UInventory::GetSlot(int32 index)
{
	if (!IsSetUp || index < 0 || index >= Slots.Num()) return *(new FInventorySlot);
	
	return *Slots[index];
}

FInventorySlot* UInventory::GetSlotRef(int32 index)
{
	if (!IsSetUp || index < 0 || index >= Slots.Num()) return nullptr;

	return Slots[index];
}

FItemStructure* UInventory::GetItemStructure(int32 ItemID)
{
	return TableOfItems->FindRow<FItemStructure>(FName(FString::FromInt(ItemID)), Context);
}

void UInventory::Empty(int32 SlotID)
{
	Slots[SlotID]->ItemID = -1;
	Slots[SlotID]->CurrentStack = 0;
	Slots[SlotID]->MaxStack = 0;
}

void UInventory::RemoveFrom(int32 SlotID, int32 Amount)
{
	if (Slots[SlotID]->CurrentStack > Amount)
	{
		//UE_LOG(LogTemp, Warning, TEXT("RemoveFrom(): CurrentStack > Amount"));
		Slots[SlotID]->CurrentStack -= Amount;
	}
	else if (Slots[SlotID]->CurrentStack == Amount)
	{
		//UE_LOG(LogTemp, Warning, TEXT("RemoveFrom(): CurrentStack == Amount"));
		Empty(SlotID);
	}
	else UE_LOG(LogTemp, Warning, TEXT("In UInventory::RemoveFrom(): tried to remove too much: Amount: %i, CurrentStack: %i, SlotID: %i"), Amount, Slots[SlotID]->CurrentStack, SlotID)
}

int32 UInventory::Num()
{
	return IsSetUp? MaxSlots : 0;
}

void UInventory::Swap(int32 Slot1, int32 Slot2)
{
	if (!IsSetUp) return;

	FInventorySlot* TempSlot;
	TempSlot = Slots[Slot1];
	Slots[Slot1] = Slots[Slot2];
	Slots[Slot2] = TempSlot;
}

int UInventory::Stack(int32 Slot1, int32 Slot2)
{
	if (!IsSetUp) return 0;

	if (Slot1 == Slot2) return 0; // base case; this prevents duplication

	FInventorySlot* Source = Slots[Slot1];
	FInventorySlot* Dest = Slots[Slot2];

	int32 ToAdd = (Source->CurrentStack + Dest->CurrentStack > Dest->MaxStack) ? Dest->MaxStack - Dest->CurrentStack : Source->CurrentStack;

	Dest->CurrentStack += ToAdd;
	Source->CurrentStack -= ToAdd;

	if (Source->CurrentStack == 0)
	{
		Empty(Slot1);
	}

	return Source->CurrentStack;
}

void UInventory::UpdateSlot(FInventorySlot* UpdatedSlot, int32 SlotNum)
{
	if (SlotNum >= Num() || SlotNum < 0) return;
	
	Slots[SlotNum] = UpdatedSlot;
}

bool UInventory::IsAttribute(int32 ItemID)
{
	FItemStructure* Row = TableOfItems->FindRow<FItemStructure>(FName(FString::FromInt(ItemID)), Context);
	return Row->is_attribute;
}

void UInventory::BeginPlay()
{
	Super::BeginPlay();
}

void UInventory::CreateSlots()
{
	for (int i = 0; i < MaxSlots; i++)
	{
		Slots.Add(new FInventorySlot);
	}
}

bool UInventory::HasSpace(int32 ItemID, int32 Amount, int32 MaxStack)
{
	if (!IsSetUp) return false;

	// loop through array and find if there are any Slots that have this item id
	int32 LeftToFind = Amount;
	for (int i = 0; i < MaxSlots; i++)
	{
		if (Slots[i]->ItemID == ItemID)
		{
			LeftToFind -= (Slots[i]->MaxStack - Slots[i]->CurrentStack);
		}
		else if (Slots[i]->ItemID == -1)
		{
			LeftToFind -= MaxStack;
		}
		if (LeftToFind <= 0) return true;
	}
	return false;
}

void UInventory::AddToStack(const int32& ItemID, int32& LeftToAdd)
{
	for (int i = 0; i < MaxSlots; i++)
	{
		if (Slots[i]->ItemID == ItemID && Slots[i]->CurrentStack < Slots[i]->MaxStack)
		{
			int32 ToAdd = (Slots[i]->MaxStack - Slots[i]->CurrentStack) > LeftToAdd ? LeftToAdd : (Slots[i]->MaxStack - Slots[i]->CurrentStack);
			LeftToAdd -= ToAdd;
			Slots[i]->CurrentStack += ToAdd;
		}

		if (LeftToAdd <= 0) return;
	}
}

void UInventory::Setup(UDataTable* DT, int32 NumSlots, bool IsUser)
{
	TableOfItems = DT;
	MaxSlots = NumSlots;
	OwnedByUser = IsUser;
	CreateSlots();

	IsSetUp = true;
}

void UInventory::AddFromTopDown(int32& ItemID, int32& LeftToAdd, const int32& MaxStack)
{
	for (int i = 0; i < MaxSlots; i++)
	{
		if (Slots[i]->ItemID == -1)
		{
			Slots[i]->ItemID = ItemID;
			Slots[i]->MaxStack = MaxStack;
			int32 ToAdd = (MaxStack > LeftToAdd) ? LeftToAdd : MaxStack;
			LeftToAdd -= ToAdd;
			Slots[i]->CurrentStack = ToAdd;
		}
		if (LeftToAdd <= 0) return;
	}
}

/*
* The way I want the inventory to work is when adding from the bottom, don't just start with the last slot.
* The first slot to check is the left-most slot on the bottom, which in this case is Slots[Max - 10 - 1]
* So we have to start with that slot and work our way to the right, looking for an empty space. If none are found,
* repeat on the second-to-last row.
*/
void UInventory::AddFromBottomUp(int32& ItemID, int32& LeftToAdd, const int32& MaxStack)
{
	// 40 slots total
	// yes, this is hardcoded. I may fix it later if I feel like it.
	TArray<int> Intervals;
	Intervals.Add(40);
	Intervals.Add(30);
	Intervals.Add(20);
	Intervals.Add(10);

	for (int j = 0; j < Intervals.Num(); j++)
	{
		for (int i = Intervals[j] - 10; i < Intervals[j]; i++)
		{
			if (Slots[i]->ItemID == -1)
			{
				Slots[i]->ItemID = ItemID;
				Slots[i]->MaxStack = MaxStack;
				int32 ToAdd = (MaxStack > LeftToAdd) ? LeftToAdd : MaxStack;
				LeftToAdd -= ToAdd;
				Slots[i]->CurrentStack = ToAdd;
			}
			if (LeftToAdd <= 0) return;
		}
	}
}

void UInventory::PrintInventory()
{
	for (int i = 0; i < Slots.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slots[%i]: <%i, %i, %i>"), i, Slots[i]->ItemID, Slots[i]->CurrentStack, Slots[i]->MaxStack);
	}
}
