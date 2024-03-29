// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Container.generated.h"

class UCapsuleComponent;
class UInventory;
class UDataTable;


UCLASS()
class SLASH_API AContainer : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:
	AContainer();
	virtual void Interact(USlashOverlay* Overlay, UAttributeComponent* Attributes) override;
	virtual FString GetActorName() override;
	virtual FString GetInteractWord() override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* InventoryDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventory* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Spawn")
	TArray<int32> ItemsToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Spawn")
	TArray<int32> MinItemsToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Spawn")
	TArray<int32> MaxItemsToSpawn;

private:
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	void AddStartItems();
};
