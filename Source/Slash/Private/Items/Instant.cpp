// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Instant.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

void AInstant::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
		}

		SpawnPickupSystem(); // Inherited from Item; if there is a Niagara System, spawn it
		Overlapped(); // overlap behavior of instants - in this case it will destroy the object
	}
}

void AInstant::Overlapped()
{
	Destroy();
}