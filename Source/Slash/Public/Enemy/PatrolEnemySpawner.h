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
	
	UPROPERTY(EditAnywhere, Category = "Spawner Defaults")
	TArray<TSubclassOf<AEnemy>> EnemyClasses;

	UPROPERTY(EditAnywhere, Category = "Spawner Defaults")
	FVector EnemySpawnHalfSize = FVector(200.f, 200.f, 0.f);
	
	// How far off the ground the enemy should spawn (used to avoid collisions)
	UPROPERTY(EditAnywhere, Category = "Spawner Defaults")
	float SpawnOffGround = 100.f;

	// How far the line trace should trace in each direction
	UPROPERTY(EditAnywhere, Category = "Spawner Defaults")
	float DistanceToTrace = 600.f;

	UPROPERTY(EditAnywhere, Category = "Spawner Defaults")
	FVector TargetSpawnHalfSize = FVector(2000.f, 2000.f, 0.f);
	float TargetRadius = 2000.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	AEnemy* SpawnEnemy(FVector Origin);
	ATargetPoint* SpawnTargetPoint(FVector Origin);
	FHitResult GetGroundLevelZ(FVector Origin);
	void BuildTargetPointsArray(TArray<ATargetPoint*>& TempPoints);

};
