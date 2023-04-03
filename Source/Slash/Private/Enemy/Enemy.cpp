// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Subsystems/EventsSubsystem.h"
#include "Components/QuestObjectiveTypes.h"
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapon.h"
#include "Items/Soul.h"

// public
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetPeripheralVisionAngle(45.f);
	PawnSensing->SightRadius = 4000.f;
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead()) return;
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	
	if (IsOutsideAttackRadius())
	{
		StartChaseTarget();
	}
	else
	{
		//EnemyState = EEnemyState::EES_Attacking;
		StartAttackTimer();
	}

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}

	if (ExtraWeapons.Num())
	{
		for(AWeapon* Wep : ExtraWeapons)
		{
			Wep->Destroy();
		}
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (!IsDead()) ShowHealthBar();
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	ClearAttackTimer();
	StopAttackMontage();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SetExtraWeaponCollisionDisabled();
}

//protected
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	EnemyController = Cast<AAIController>(GetController());
	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	HideHealthBar();
	MoveToTarget(PatrolTarget);
	SpawnDefaultWeapons();
	EventsSubsystem = GetGameInstance()->GetSubsystem<UEventsSubsystem>();
}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(false);

	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(5.f);
	//GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SetExtraWeaponCollisionDisabled();
	SpawnSoul();
	if (EventsSubsystem) EventsSubsystem->CreateEvent(EObjectiveType::EOT_EnemyKilled, EnemyID, 1);
}

void AEnemy::Attack()
{
	Super::Attack();

	if (CombatTarget == nullptr) return;
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	return !IsOutsideCombatRadius() && (!IsAttacking() || !GetWorldTimerManager().IsTimerActive(AttackTimer)) && !IsDead() && !IsEngaged();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::AttackEnd()
{
	//UE_LOG(LogTemp, Warning, TEXT("In Enemy::AttackEnd()"));
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::SetExtraWeaponCollisionEnabled(int32 WeaponIndex)
{
	// only accepts enabled; enables the weapon at weapon index (if it's not bigger than the size of the array)
	// and sets that weapon as the ActiveExtraWeapon
	if (WeaponIndex < ExtraWeapons.Num())
	{
		if (ExtraWeapons[WeaponIndex]->GetWeaponBox())
		{
			ExtraWeaponInUse = ExtraWeapons[WeaponIndex];
			ExtraWeaponInUse->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // is QueryOnly correct? ABP will know
			ExtraWeaponInUse->IgnoreActors.Empty();
		}
	}
}

void AEnemy::SetExtraWeaponCollisionDisabled()
{
	if (ExtraWeaponInUse)
	{
		ExtraWeaponInUse->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// private
/*
 * Patrolling is prone to breaking. A possible solution:
 * If no moverequest && no patrol timer active OR if moverequest is incomplete && enemy velocity is 0 (Enemy needs a kick):
 *   Cancel existing moverequest (if applicable), grab a new patrol point and create a new moverequest
 *   Also, might add an actual "is navigable" check before executing the moverequest
 *   Also also, all over the place we're assuming that the PatrolTarget is set; we should not do that. Right before RequestMove() (or whatever)
 *   we should check if PatrolTarget is set and if not, just set it! Don't let Patrolling get in a bad state just because there's no PatrolTarget,
 *   how silly
 */
void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, FMath::RandRange(PatrolWaitMin, PatrolWaitMax));
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		// Called in the case that Character is behind Enemy or ALSO character is being chased because of PawnSeen()
		// In the latter case, every .5s the Enemy will lose interest in the character and then regain interest through PawnSeen()
		//UE_LOG(LogTemp, Warning, TEXT("In CheckCombatTarget(): IsOutsideCombatRadius()"));
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged()) StartPatrolling();
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		// Enemy is probably Attacking or Engaging and and Character has moved outside of Attack Radius
		//UE_LOG(LogTemp, Warning, TEXT("In CheckCombatTarget(): IsOutsideAttackRadius() && !IsChasing()"));
		ClearAttackTimer();
		if (!IsEngaged()) StartChaseTarget();
	}
	else if (CanAttack())
	{
		// Enemy is inside AttackRadius, Enemy may be Patrolling and turned away
		//UE_LOG(LogTemp, Warning, TEXT("In CheckCombatTarget(): Attacking"));
		ClearPatrolTimer();
		if (!IsEngaged()) StartAttackTimer();
	}
}

AActor* AEnemy::ChoosePatrolTarget()
{
	// if there's only 1 target, then the new target & current target will always be equal
	if (PatrolPoints.Num() <= 1) return nullptr;

	// choose a new target, compare to old target. If equal, select a new target
	AActor* NewTarget = PatrolPoints[FMath::RandRange(0, PatrolPoints.Num() - 1)];
	while (NewTarget == PatrolTarget)
	{
		NewTarget = PatrolPoints[FMath::RandRange(0, PatrolPoints.Num() - 1)];
	}
	return NewTarget;
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	ShowHealthBar();
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	//UE_LOG(LogTemp, Warning, TEXT("DistanceRadius, CombatRadius: %d, %d"), (CombatTarget->GetActorLocation() - GetActorLocation()).Size(), CombatRadius);
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	return (Target->GetActorLocation() - GetActorLocation()).Size() <= Radius;
}

void AEnemy::MoveToTarget(AActor* TheTarget, double Distance, FColor DebugColor)
{
	if (EnemyController == nullptr || TheTarget == nullptr) return;

	FAIMoveRequest MoveRequest;
	FNavPathSharedPtr NavPath;

	double AcceptanceDistance = Distance = 0.f ? AcceptanceRadius : Distance;

	MoveRequest.SetGoalActor(TheTarget);
	MoveRequest.SetAcceptanceRadius(AcceptanceDistance);

	EnemyController->MoveTo(MoveRequest, &NavPath);

	/*TArray<FNavPathPoint>& PathPoint = NavPath->GetPathPoints();
	for (auto Point : PathPoint)
	{
		DrawDebugSphere(GetWorld(), Point.Location, 3.f, 8, DebugColor, false, 10.f);
	}*/
}

void AEnemy::SpawnDefaultWeapons()
{
	if (GetWorld() && WeaponClass)
	{
		AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}

	// Note: Both WeaponClasses AND WeaponSockets arrays must be filled out AND THE SAME LENGTH
	// or this code will not be executed
	if (WeaponClasses.Num() > 0 && WeaponClasses.Num() == WeaponSockets.Num())
	{
		for (int i = 0; i < WeaponClasses.Num(); i++)
		{
			AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClasses[i]);
			DefaultWeapon->Equip(GetMesh(), WeaponSockets[i], this, this);
			ExtraWeapons.Add(DefaultWeapon);
		}
	}
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::SpawnSoul()
{
	if (GetWorld() && SoulClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);
		ASoul* SpawnedSoul = GetWorld()->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
		if (SpawnedSoul)
		{
			SpawnedSoul->SetSouls(Attributes->GetSouls());
			SpawnedSoul->SetOwner(this);
		}
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	//UE_LOG(LogTemp, Warning, TEXT("In PawnSeen()"));
	if (Tags.Num() == 0 || SeenPawn->ActorHasTag(Tags[0])) return;
	//UE_LOG(LogTemp, Warning, TEXT("In PawnSeen(): Pawn passed Tag check"));
	const bool bShouldChaseTarget =
		EnemyState <= EEnemyState::EES_Chasing &&
		EnemyState != EEnemyState::EES_Dead;

	if (bShouldChaseTarget)
	{
		//UE_LOG(LogTemp, Warning, TEXT("In PawnSeen(): Enemy will chase target"));
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		StartChaseTarget();
	}
}