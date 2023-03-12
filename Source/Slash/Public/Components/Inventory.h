// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Inventory.generated.h"

class AItem;
class UTexture2D;

USTRUCT(Blueprintable)
struct FItemStructure : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int item_id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AItem> class_ref;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName display_name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ui_icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool is_attribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> items_taken_when_purchased;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> amount_taken_when_purchased;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> items_added_when_selling;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> amount_added_when_selling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int max_stack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool usable;
};

USTRUCT(Blueprintable)
struct FInventorySlot
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentStack = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStack = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventory();

	UFUNCTION(BlueprintCallable)
	bool AddStrict(int32 ItemID, int32 Amount);

	UFUNCTION(BlueprintCallable)
	TSubclassOf<AItem> AddLenient(int32 ItemID, int32& Amount);

	UFUNCTION(BlueprintCallable)
	FInventorySlot GetSlot(int32 index);
	
	UFUNCTION(BlueprintCallable)
	int32 Num();

	bool IsAttribute(int32 ItemID);
	void SetDataTable(UDataTable* DT);

protected:
	virtual void BeginPlay() override;

private:
	bool HasSpace(int32 ItemID, int32 Amount, int32 MaxStack);
	void AddToStack(const int32& ItemID, int32& LeftToAdd);
	void AddFromTopDown(int32& ItemID, int32& LeftToAdd, const int32& MaxStack);
	void AddFromBottomUp(int32& ItemID, int32& LeftToAdd, const int32& MaxStack);
	void PrintInventory();

	UDataTable* TableOfItems;

	TArray<FInventorySlot*> Slots;

	int32 MaxSlots = 40;

	const FString Context = "InventoryItems";

};
