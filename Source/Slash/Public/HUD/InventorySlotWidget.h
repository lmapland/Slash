// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UInventory;
/**
 * 
 */
UCLASS()
class SLASH_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/* This class exists because I didn't want to write this function in Blueprints */
	UFUNCTION(BlueprintCallable)
	void MoveAcrossInventories(int32 SourceSlotIndex, int32 DestSlotIndex, UInventory* SourceInventory, UInventory* DestInventory);
};