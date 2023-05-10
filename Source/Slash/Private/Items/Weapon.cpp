// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/StaticMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(ItemMesh);
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	// disable collision on item mesh, in case the static mesh comes with some static collision or something
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(ItemMesh);

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(ItemMesh);

	Damage = 25.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}


// Did we hit an enemy with the weapon?
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// if the weapon owner and the damage receiver are in the same group they do not damage each other
	FName MyGroup(""); // prevent crashes if no tags are set
	if (GetOwner()->Tags.Num() > 0)
	{
		MyGroup = GetOwner()->Tags[0];
	}
	if (OtherActor->ActorHasTag(MyGroup)) return;

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	if (BoxHit.GetActor())
	{
		// There's a chance that the OtherActor IS of a different group but the BoxTrace found a mob of the SAME group
		// so, need another check here
		if (BoxHit.GetActor()->ActorHasTag(MyGroup)) return;
		float DamageToDo = Damage * DamageModifier;
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), DamageToDo, GetInstigator()->GetController(), this, UDamageType::StaticClass());

		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
	}
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	DisableSphereCollision();

	DeactivateEmbers();
}

void AWeapon::DeactivateEmbers()
{
	if (ItemParticles)
	{
		ItemParticles->Deactivate();
	}
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquipSound, GetActorLocation());
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::ActivateTrailEffect()
{
	if (TrailEffect)
	{
		// to get a middle location for spawning, need to get the two sockets, add togeher & divide in half
		FVector TopSocketLocation = ItemMesh->GetSocketLocation(TEXT("TrailTop"));
		FVector BottomSocketLocation = ItemMesh->GetSocketLocation(TEXT("TrailBottom"));
		FVector LocationToSpawn = ((TopSocketLocation + BottomSocketLocation) / 2);

		// to get appropriate rotation, use the Kismet Math Library
		FRotator RotationToSpawn = UKismetMathLibrary::MakeRotFromZ((TopSocketLocation - BottomSocketLocation));

		SpawnedNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailEffect, GetMesh(), FName(""), LocationToSpawn, RotationToSpawn, EAttachLocation::KeepWorldPosition, true);

		// to get length of blade for length of trail, subtract bottom from top and get length of resulting vector
		// where to use, though?
		double SystemLength = (TopSocketLocation - BottomSocketLocation).Length();

		SpawnedNiagaraComponent->SetFloatParameter(FName("TrailWidth"), SystemLength);
	}
}

void AWeapon::DeactivateTrailEffect()
{
	if (SpawnedNiagaraComponent)
	{
		SpawnedNiagaraComponent->Deactivate();
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	// For a single swing: ignore this actor if we hit them again so we don't accidentally apply damage twice
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Start, End, BoxTraceExtent, BoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, BoxHit, true);
	if (BoxHit.GetActor())
	{
		IgnoreActors.AddUnique(BoxHit.GetActor());
	}
}