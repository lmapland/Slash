// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetupAttachment(GetRootComponent());
	
	ItemParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemParticles"));
	ItemParticles->SetupAttachment(ItemMesh);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

void AItem::SpawnPickupSystem()
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupEffect, GetActorLocation());
	}
}

void AItem::PlayPickupSound()
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), PickupSound);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}

}

void AItem::PickedUp()
{
	Destroy();
}

void AItem::PickUp()
{
	SpawnPickupSystem();
	PickedUp();
	PlayPickupSound();
	//GetWorldTimerManager().SetTimer(DestroySelfTimer, this, &AItem::PickedUp, 0.1f);
}

void AItem::Interact(USlashOverlay* Overlay, UAttributeComponent* Attributes)
{
}

FString AItem::GetActorName()
{
	return ItemName;
}

FString AItem::GetInteractWord()
{
	return FString("Pick up");
}
