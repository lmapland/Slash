// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolEnemySpawner.generated.h"

class AEnemy;
class ATargetPoint;

UCLASS()
class SLASH_API APatrolEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolEnemySpawner();
	
	// The list of Enemies that can possibly be spawned. Must be subclasses of AEnemy
	UPROPERTY(EditAnywhere, Category = "Spawner Defaults")
	TArray<TSubclassOf<AEnemy>> EnemyClasses;

	// With the spawner as the Origin, the radius of the bounding box that the enemy will be spawned in
	UPROPERTY(EditAnywhere, Category = "Spawner Defaults")
	float EnemyRadius = 1000.f;
	
	// How far off the ground the enemy should spawn (used to avoid collisions)
	UPROPERTY(EditAnywhere, Category = "Spawner Defaults")
	float SpawnOffGround = 100.f;

	// How far the line trace should trace up and down (one trace down, then one trace up)
	UPROPERTY(EditAnywhere, Category = "Spawner Defaults")
	float DistanceToTrace = 600.f;

	// With the spawned enemy as the Origin, the radius of the bounding box that all patrol points will be spawned in
	UPROPERTY(EditAnywhere, Category = "Spawner Defaults")
	float TargetRadius = 2000.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	AEnemy* SpawnEnemy(FVector Origin);
	ATargetPoint* SpawnTargetPoint(FVector Origin);
	FHitResult GetGroundLevelZ(FVector Origin);
	void BuildTargetPointsArray(TArray<ATargetPoint*>& TempPoints, FVector Origin);

	// Prevents the patrol points from being spawned too close together for the patrolling logic to work
	float DistBetweenPatrolPoints = 200.f;

	FTimerHandle PatrolTimer;

};
