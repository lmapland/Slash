// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/LandscapeResource.h"
#include "Components/CapsuleComponent.h"
#include "Interfaces/PickupInterface.h"

// Sets default values
ALandscapeResource::ALandscapeResource()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetupAttachment(GetRootComponent());

}

void ALandscapeResource::PickedUp()
{
	Destroy();
}

void ALandscapeResource::Pick()
{
	bPickable = false;
	GetWorldTimerManager().SetTimer(DestroySelfTimer, this, &ALandscapeResource::PickedUp, 2.5f);
}

void ALandscapeResource::Interact(USlashOverlay* Overlay, UAttributeComponent* Attributes)
{
}

FString ALandscapeResource::GetActorName()
{
	return ResourceName;
}

FString ALandscapeResource::GetInteractWord()
{
	return InteractWord;
}

// Called when the game starts or when spawned
void ALandscapeResource::BeginPlay()
{
	Super::BeginPlay();

}

