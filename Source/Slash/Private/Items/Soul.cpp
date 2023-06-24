// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ASoul::ASoul()
{
	ItemState = EItemState::EIS_Hovering;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(ItemMesh);
}

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetActorLocation().Z > DesiredZ)
	{
		const FVector DeltaLocation = FVector(0.f, 0.f, DriftRate * DeltaTime);
		AddActorWorldOffset(DeltaLocation);
	}
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();
	const FVector StartLocation = GetActorLocation();
	const FVector EndLocation = StartLocation - FVector(0.f, 0.f, 2000.f);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	FHitResult HitResult;
	
	UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), StartLocation, EndLocation, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

	DesiredZ = HitResult.ImpactPoint.Z + 50.f;

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ASoul::OnSphereOverlap);
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("ASoul::OnSphereOverlap()"));
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	
	if (PickupInterface)
	{
		PickUp();
		PickupInterface->AddItem(ItemID, Amount);
	}
}
