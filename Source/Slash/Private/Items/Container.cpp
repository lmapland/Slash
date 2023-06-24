// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Container.h"
#include "Components/Inventory.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/SlashOverlay.h"

AContainer::AContainer()
{
	PrimaryActorTick.bCanEverTick = false;


	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	Mesh->SetupAttachment(GetRootComponent());

	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));

}

void AContainer::Interact(USlashOverlay* Overlay, UAttributeComponent* Attributes)
{
	Overlay->OpenEverythingMenu(Attributes->GetSouls(), Attributes->GetSoulsUntilNextLevel(), Attributes->GetLevel() + 1, Inventory);
}

FString AContainer::GetActorName()
{
	return FString("Container");
}

FString AContainer::GetInteractWord()
{
	return FString("Open");
}

void AContainer::BeginPlay()
{
	Super::BeginPlay();

	Inventory->Setup(InventoryDataTable);

	AddStartItems();

}

void AContainer::AddStartItems()
{
	for (int32 i = 0; i < ItemsToSpawn.Num(); i++)
	{
		const int32 MinAmount = MinItemsToSpawn.Num() > i ? MinItemsToSpawn[i] : 0;
		const int32 MaxAmount = MaxItemsToSpawn.Num() > i ? MaxItemsToSpawn[i] : 0;
		int32 NumToSpawn = FMath::RandRange(MinAmount, MaxAmount);
		Inventory->AddLenient(ItemsToSpawn[i], NumToSpawn);
	}
}
