// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/NPC.h"
#include "HUD/SlashOverlay.h"

// Sets default values
ANPC::ANPC()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ANPC::Interact(USlashOverlay* SlashOverlay)
{
	// adds its own interface to the SlashOverlay
	// build "seller" interface
	// give this dude a reference to the interface... maybe the overlay actually has it already but it's hidden?
	// interface has a list, maybe a max of 5 items because I'm feeling lazy, and if you click it gives more info
	//  where does the item name come from? This class, probably. New class, items for sale?

	// call method on SlashOverlay here and pass it SaleItems
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
}

