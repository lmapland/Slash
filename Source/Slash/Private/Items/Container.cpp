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

void AContainer::BeginPlay()
{
	Super::BeginPlay();

	Inventory->Setup(InventoryDataTable);

}
