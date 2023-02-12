// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

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
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	
private:
	// AI Behavior
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void CheckCombatTarget2();
	AActor* ChoosePatrolTarget();
	void PatrolTimerFinished();
	void LoseInterest();
	void StartPatrolling();
	void StartChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	//bool IsPatrolling();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* TheTarget, double Distance = 0.f, FColor DebugColor = FColor::Green);
	void SpawnDefaultWeapons();
	void HideHealthBar();
	void ShowHealthBar();
	void SpawnSoul();


	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	// AI Behavior
	UPROPERTY(VisibleAnywhere)
	class UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;
	
	/* If the enemy hits with more than 1 weapon, this handles that case */

	// holds the list of Weapon Classes to use for each body part
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AWeapon>> WeaponClasses;

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
	double CombatRadius = 500.0;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.0;

	// For the MoveToTarget function: how close the Actor needs to be to the Target for the Move to be complete
	UPROPERTY(EditAnywhere)
	double AcceptanceRadius = 30.f;

	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

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
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 8.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> SoulClass;

};
