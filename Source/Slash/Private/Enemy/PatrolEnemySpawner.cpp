// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/PatrolEnemySpawner.h"
#include "Engine/TargetPoint.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy/Enemy.h"

// Sets default values
APatrolEnemySpawner::APatrolEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APatrolEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyClasses.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("PatrolEnemySpawner: No Enemy Class References were given; exiting"));
		return;
	}

	AEnemy* ChosenEnemy = SpawnEnemy(GetActorLocation());
	if (!ChosenEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("PatrolEnemySpawner: No floor could be found for the enemy; not spawning"));
		return;
	}

	TArray<ATargetPoint*> TempPoints;
	BuildTargetPointsArray(TempPoints);

	for (auto Point : TempPoints)
	{
		ChosenEnemy->AddPatrolPoint(Point);
	}

	// Set the PatrolPoint & launch the Enemy into it's Patrol pattern
	if (TempPoints.Num() > 0)
	{
		ChosenEnemy->SetFirstPatrolPoint(TempPoints[FMath::RandRange(0, TempPoints.Num() - 1)]);
		ChosenEnemy->StartPatrolling();
	}
	
}

void APatrolEnemySpawner::BuildTargetPointsArray(TArray<ATargetPoint*>& TempPoints)
{
	// Get and add Target Point to Enemy's PatrolPoint
	float Difference = (TargetRadius - 200.f) / 2;

	// upper right
	ATargetPoint* Target = SpawnTargetPoint(GetActorLocation() + FVector(Difference, Difference, 0.f));
	if (Target)
	{
		TempPoints.Add(Target);
	}

	// bottom right
	Target = SpawnTargetPoint(GetActorLocation() + FVector(Difference, -Difference, 0.f));
	if (Target)
	{
		TempPoints.Add(Target);
	}

	// bottom left
	Target = SpawnTargetPoint(GetActorLocation() + FVector(-Difference, -Difference, 0.f));
	if (Target)
	{
		TempPoints.Add(Target);
	}

	// upper left
	Target = SpawnTargetPoint(GetActorLocation() + FVector(-Difference, Difference, 0.f));
	if (Target)
	{
		TempPoints.Add(Target);
	}
}

AEnemy* APatrolEnemySpawner::SpawnEnemy(FVector Origin)
{
	// Choose the enemy to spawn
	TSubclassOf<AEnemy> ChosenEnemy = EnemyClasses[FMath::RandRange(0, EnemyClasses.Num() - 1)];

	// Choose where to spawn the enemy
	FVector XYVector = UKismetMathLibrary::RandomPointInBoundingBox(Origin, EnemySpawnHalfSize);
	FHitResult HitResult = GetGroundLevelZ(XYVector);

	// HitResult might not have found a valid hit
	if (!HitResult.bBlockingHit)
	{
		return nullptr;
	}

	FVector LocationToSpawn = FVector(Origin.X, Origin.Y, SpawnOffGround) + FVector(0.f, 0.f, HitResult.ImpactPoint.Z);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//UKismetSystemLibrary::DrawDebugSphere(this, LocationToSpawn, 25.f, 8, FLinearColor::Red, 30.f);

	// Spawn the enemy
	return GetWorld()->SpawnActor<AEnemy>(ChosenEnemy, LocationToSpawn, FRotator(), SpawnParams);
}

ATargetPoint* APatrolEnemySpawner::SpawnTargetPoint(FVector Origin)
{
	FVector XYVector = UKismetMathLibrary::RandomPointInBoundingBox(Origin, TargetSpawnHalfSize);
	FHitResult HitResult = GetGroundLevelZ(XYVector);
	
	// HitResult might not have found a valid hit
	if (!HitResult.bBlockingHit)
	{
		return nullptr;
	}

	FVector LocationToSpawn = FVector(Origin.X, Origin.Y, 0.f) + FVector(0.f, 0.f, HitResult.ImpactPoint.Z);
	//UKismetSystemLibrary::DrawDebugSphere(this, LocationToSpawn, 25.f, 8, FLinearColor::Red, 30.f);

	return GetWorld()->SpawnActor<ATargetPoint>(LocationToSpawn, FRotator());
}

FHitResult APatrolEnemySpawner::GetGroundLevelZ(FVector Origin)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	FHitResult HitResult;

	// Trace down first
	UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Origin, Origin - FVector(0.f, 0.f, DistanceToTrace), ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true);
	
	if (HitResult.bBlockingHit)
	{
		return HitResult;
	}

	// If no hit is found, perhaps we are under the surface. Trace up.
	UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Origin, Origin + FVector(0.f, 0.f, DistanceToTrace), ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true);
	
	return HitResult;
}
