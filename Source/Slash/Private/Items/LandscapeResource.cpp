// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/LandscapeResource.h"
#include "Components/SphereComponent.h"
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

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(ItemMesh);
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

void ALandscapeResource::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->SetOverlappingResource(this);
	}
}

void ALandscapeResource::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->SetOverlappingResource(nullptr);
	}
}

// Called when the game starts or when spawned
void ALandscapeResource::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ALandscapeResource::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &ALandscapeResource::OnSphereEndOverlap);
}

