// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/NPC.h"
#include "Interfaces/Interactable.h"
#include "Merchant.generated.h"

class AItem;

/**
 * 
 */
UCLASS()
class SLASH_API AMerchant : public ANPC, public IInteractable
{
	GENERATED_BODY()
public:
	virtual void Interact(USlashOverlay* Overlay, UAttributeComponent* Attributes) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selling")
	TArray<int32> ItemsToSell;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selling")
	TArray<int32> ItemsToBuy;

};
