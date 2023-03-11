// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Inventory.h"

UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
}

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
	PrintInventory();
	return true;
}

TSubclassOf<AItem> UInventory::AddLenient(int32 ItemID, int32& Amount)
{
	FItemStructure* Row = TableOfItems->FindRow<FItemStructure>(FName(FString::FromInt(ItemID)), Context);

	// Add items
	int32 LeftToAdd = Amount;
	int32 MaxStack = Row->max_stack;

	// First add to any stacks that already exist
	AddToStack(ItemID, Amount);
	if (Amount <= 0) return nullptr;

	// either no stacks exist or the amount is bigger than the current stack can hold
	if (Row->usable) { AddFromTopDown(ItemID, Amount, MaxStack); }
	else { AddFromBottomUp(ItemID, Amount, MaxStack); }

	UE_LOG(LogTemp, Warning, TEXT("AddLenient(): Done adding, %i left."), Amount);
	//PrintInventory();
	return Row->class_ref;
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

bool UInventory::IsAttribute(int32 ItemID)
{
	FItemStructure* Row = TableOfItems->FindRow<FItemStructure>(FName(FString::FromInt(ItemID)), Context);
	return Row->is_attribute;
}

void UInventory::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < MaxSlots; i++)
	{
		Slots.Add(new FInventorySlot);
	}
}

bool UInventory::HasSpace(int32 ItemID, int32 Amount, int32 MaxStack)
{
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

void UInventory::SetDataTable(UDataTable* DT)
{
	TableOfItems = DT;
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
