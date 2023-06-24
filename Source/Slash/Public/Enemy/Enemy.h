// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "Engine/DataTable.h"
#include "Enemy.generated.h"

USTRUCT(Blueprintable)
struct FEnemyStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemy> ClassRef;
};

class AWeapon;
class UEventsSubsystem;
class USphereComponent;
class UHealthBarComponent;
class UCombatStateComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Hearing;
struct FAIStimulus;
struct FAIRequestID;
struct FPathFollowingResult;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, float DamageDealt, AActor* Hitter) override;
	/** </IHitInterface> */

	UFUNCTION(BlueprintCallable)
	void StartPatrolling();

	UFUNCTION()
	void OnPerception(AActor* Actor, FAIStimulus Stimulus);

	float GetForwardAngleToTarget(AActor* Target);

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;
	/** <ABaseCharacter> */

	UFUNCTION(BlueprintCallable)
	void SetExtraWeaponCollisionEnabled(int32 WeaponIndex);
	
	UFUNCTION(BlueprintCallable)
	void SetExtraWeaponCollisionDisabled();
	
	UFUNCTION()
	void OnAggroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnAggroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Navigation")
	TArray<AActor*> PatrolPoints;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI Navigation")
	AActor* PatrolTarget;

	
private:
	// AI Behavior
	void CheckPatrolTarget();
	void CheckCombatTarget();
	AActor* ChoosePatrolTarget();
	void PatrolTimerFinished();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	bool IsAlerting();
	bool IsInCombat();
	void ClearPatrolTimer();
	void ClearAttackTimer();
	void ClearAlertTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* TheTarget, double Distance = 0.f, FColor DebugColor = FColor::Green);
	void SpawnDefaultWeapons();
	void HideHealthBar();
	void ShowHealthBar();
	void SetCombatIndicatorWidgetState(int32 CombatState);
	void SpawnSoul();
	void UpdateEnemyState(EEnemyState State);
	bool FindNavigablePath(FVector DesiredLocation);
	void MoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	void SetCombatTarget(ABaseCharacter* NewTarget);

	// AI Behavior
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UCombatStateComponent* CombatStateWidget;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* AggroSphere;
	
	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerceptionComponent;

	UAISenseConfig_Hearing* Hearing;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;
	
	/* If the enemy hits with more than 1 weapon, this handles that case */

	// holds the list of Weapon Classes to use for each body part
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AWeapon>> WeaponClasses;

	// holds the corresponding sockets those Weapons should be attached
	UPROPERTY(EditAnywhere)
	TArray<FName> WeaponSockets;
	
	// holds the actual extra weapons created
	UPROPERTY(VisibleAnywhere)
	TArray<AWeapon*> ExtraWeapons;
	
	/* 
	 * basically "EquippedWeapon", but for the extra weapons.
	 * The last extra weapon to be enabled is held here so it can be easily disabled
	*/
	UPROPERTY(VisibleAnywhere)
	AWeapon* ExtraWeaponInUse;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.0;

	// For the MoveToTarget function: how close the Actor needs to be to the Target for the Move to be complete
	UPROPERTY(EditAnywhere)
	double AcceptanceRadius = 30.f;

	float AggroRadius = 0.f;

	/*
	* In case the mob is blind in one eye or something; allow the aggro to be different on the left and right sides
	* 0 is no angle: completely blind on one side. AggroAngleLeft should be between 0 and -180, and AggroAngleRight should be between 0 and +180
	*/
	UPROPERTY(EditAnywhere)
	double AggroAngleLeft = -60;
	
	UPROPERTY(EditAnywhere)
	double AggroAngleRight = 60;
	
	UPROPERTY(VisibleAnywhere)
	TArray<ABaseCharacter*> AggroTargets;

	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.0;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 125.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = Alerting)
	FTimerHandle AlertTimer;

	UPROPERTY(EditAnywhere, Category = Alerting)
	float AlertWaitTime = 3.f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float AlertingSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 8.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> SoulClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 EnemyID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float InterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	FVector AlertingLocation;

	/* To make sure at least part of the actor is in-range */
	float OverlapLeniency = 50.f;

	UEventsSubsystem* EventsSubsystem;


public:
	FORCEINLINE void AddPatrolPoint(AActor* Point) { PatrolPoints.Add(Point); }
	FORCEINLINE void SetFirstPatrolPoint(AActor* Point) { PatrolTarget = Point; }

};
