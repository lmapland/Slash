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
	//UKismetSystemLibrary::DrawDebugLine(this, ChosenEnemy->GetActorLocation(), ChosenEnemy->GetActorLocation() + FVector(2200.f, 0.f, 1000.f), FLinearColor::Black, 9000.f);
	//UKismetSystemLibrary::DrawDebugLine(this, ChosenEnemy->GetActorLocation(), ChosenEnemy->GetActorLocation() + FVector(-2200.f, 0.f, 1000.f), FLinearColor::Black, 9000.f);
	//UKismetSystemLibrary::DrawDebugLine(this, ChosenEnemy->GetActorLocation(), ChosenEnemy->GetActorLocation() + FVector(0.f, 2200.f, 1000.f), FLinearColor::Black, 9000.f);
	//UKismetSystemLibrary::DrawDebugLine(this, ChosenEnemy->GetActorLocation(), ChosenEnemy->GetActorLocation() + FVector(0.f, -2200.f, 1000.f), FLinearColor::Black, 9000.f);

	if (!ChosenEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("PatrolEnemySpawner: No floor could be found for the enemy; not spawning"));
		return;
	}

	if (TargetRadius <= (DistBetweenPatrolPoints * 2))
	{
		UE_LOG(LogTemp, Warning, TEXT("PatrolEnemySpawner: TargetRadius value must be greater than %f "), DistBetweenPatrolPoints * 2);
		return;
	}

	TArray<ATargetPoint*> TempPoints;
	BuildTargetPointsArray(TempPoints, ChosenEnemy->GetActorLocation());

	for (auto Point : TempPoints)
	{
		ChosenEnemy->AddPatrolPoint(Point);
	}

	// Set the PatrolPoint & launch the Enemy into it's Patrol pattern
	if (TempPoints.Num() > 0)
	{
		ChosenEnemy->SetFirstPatrolPoint(TempPoints[FMath::RandRange(0, TempPoints.Num() - 1)]);
		/* There's a weird thing where StartPatrolling sometimes gets called before enemy is ready, and enemy therefore never gets a trigger to start patrolling.
		 *  I do need to fix patrolling, but in the meantime I'll just wait until the enemy is almost certainly ready and launch the enemy then.
		 */
		GetWorldTimerManager().SetTimer(PatrolTimer, ChosenEnemy, &AEnemy::StartPatrolling, 2.f);
	}
	
}

void APatrolEnemySpawner::BuildTargetPointsArray(TArray<ATargetPoint*>& TempPoints, FVector Origin)
{
	// Get and add Target Point to Enemy's PatrolPoint
	float Difference = (TargetRadius + DistBetweenPatrolPoints) / 2;

	// upper right
	ATargetPoint* Target = SpawnTargetPoint(Origin + FVector(Difference, Difference, 0.f));
	//UKismetSystemLibrary::DrawDebugSphere(this, Origin + FVector(Difference, Difference, 0.f), 25.f, 8, FLinearColor::Blue, 9000.f);
	if (Target) TempPoints.Add(Target);

	// bottom right
	Target = SpawnTargetPoint(Origin + FVector(Difference, -Difference, 0.f));
	//UKismetSystemLibrary::DrawDebugSphere(this, Origin + FVector(Difference, -Difference, 0.f), 25.f, 8, FLinearColor::Blue, 9000.f);
	if (Target) TempPoints.Add(Target);

	// bottom left
	Target = SpawnTargetPoint(Origin + FVector(-Difference, -Difference, 0.f));
	//UKismetSystemLibrary::DrawDebugSphere(this, Origin + FVector(-Difference, -Difference, 0.f), 25.f, 8, FLinearColor::Blue, 9000.f);
	if (Target) TempPoints.Add(Target);

	// upper left
	Target = SpawnTargetPoint(Origin + FVector(-Difference, Difference, 0.f));
	//UKismetSystemLibrary::DrawDebugSphere(this, Origin + FVector(-Difference, Difference, 0.f), 25.f, 8, FLinearColor::Blue, 9000.f);
	if (Target) TempPoints.Add(Target);
}

AEnemy* APatrolEnemySpawner::SpawnEnemy(FVector Origin)
{
	// Choose the enemy to spawn
	TSubclassOf<AEnemy> ChosenEnemy = EnemyClasses[FMath::RandRange(0, EnemyClasses.Num() - 1)];

	// Choose where to spawn the enemy
	FVector XYVector = UKismetMathLibrary::RandomPointInBoundingBox(Origin, FVector(EnemyRadius, EnemyRadius, 0.f));
	FHitResult HitResult = GetGroundLevelZ(XYVector);

	// HitResult might not have found a valid hit
	if (!HitResult.bBlockingHit)
	{
		return nullptr;
	}

	FVector LocationToSpawn = FVector(XYVector.X, XYVector.Y, SpawnOffGround) + FVector(0.f, 0.f, HitResult.ImpactPoint.Z);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//UKismetSystemLibrary::DrawDebugSphere(this, LocationToSpawn, 25.f, 8, FLinearColor::Red, 30.f);

	// Spawn the enemy
	return GetWorld()->SpawnActor<AEnemy>(ChosenEnemy, LocationToSpawn, FRotator(), SpawnParams);
}

ATargetPoint* APatrolEnemySpawner::SpawnTargetPoint(FVector Origin)
{
	float BoxRadius = (TargetRadius - DistBetweenPatrolPoints) / 2;
	FVector XYVector = UKismetMathLibrary::RandomPointInBoundingBox(Origin, FVector(BoxRadius, BoxRadius, 0));
	FHitResult HitResult = GetGroundLevelZ(XYVector);
	
	// HitResult might not have found a valid hit
	if (!HitResult.bBlockingHit)
	{
		return nullptr;
	}

	FVector LocationToSpawn = FVector(XYVector.X, XYVector.Y, 0.f) + FVector(0.f, 0.f, HitResult.ImpactPoint.Z);
	//UKismetSystemLibrary::DrawDebugSphere(this, LocationToSpawn, 25.f, 8, FLinearColor::Red, 180.f);

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
	UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Origin, Origin - FVector(0.f, 0.f, DistanceToTrace), ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	
	if (HitResult.bBlockingHit)
	{
		return HitResult;
	}

	// If no hit is found, perhaps we are under the surface. Trace up.
	UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Origin, Origin + FVector(0.f, 0.f, DistanceToTrace), ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	
	return HitResult;
}
