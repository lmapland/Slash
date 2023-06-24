// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/QuestObjectiveTypes.h"
#include "Components/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Subsystems/EventsSubsystem.h"
#include "HUD/HealthBarComponent.h"
#include "HUD/CombatStateComponent.h"
#include "Items/Weapon.h"
#include "Items/Soul.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionComponent.h"
#include "Navigation/PathFollowingComponent.h"
//#include "DrawDebugHelpers.h"

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

	CombatStateWidget = CreateDefaultSubobject<UCombatStateComponent>(TEXT("CombatState"));
	CombatStateWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(GetRootComponent());

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	Hearing->HearingRange = 3000.f;
	Hearing->DetectionByAffiliation.bDetectEnemies = true;
	Hearing->DetectionByAffiliation.bDetectFriendlies = true;
	Hearing->DetectionByAffiliation.bDetectNeutrals = true;
	AIPerceptionComponent->ConfigureSense(*Hearing);
	AIPerceptionComponent->SetDominantSense(Hearing->GetSenseImplementation());
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead()) return;

	if (CombatTarget)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation());
		FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotationYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}
	else if (!CombatTarget && AggroTargets.Num() > 0 && EnemyState <= EEnemyState::EES_Alerting)
	{
		/*
		* Currently has at least one aggrotarget: Target is within the sphere radius & is out of sight
		* Want to check if any aggrotargets have moved within the sight radius
		*/
		ABaseCharacter* NewCombatTarget = nullptr;
		for (auto AggroTarget : AggroTargets)
		{
			float CurrentAngle = GetForwardAngleToTarget(AggroTarget);
			//UE_LOG(LogTemp, Warning, TEXT("Tick(): AggroAngle: %f"), CurrentAngle);
			if (CurrentAngle >= AggroAngleLeft && CurrentAngle <= AggroAngleRight)
			{
				NewCombatTarget = AggroTarget;
			}
		}
		if (NewCombatTarget)
		{
			CombatTarget = NewCombatTarget;
			UE_LOG(LogTemp, Warning, TEXT("Tick, clearing AggroTargets %s: %s"), *GetName(), *NewCombatTarget->GetName());
			UpdateEnemyState(EEnemyState::EES_Chasing);
		}
	}

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
	ABaseCharacter* Character = Cast<ABaseCharacter>(EventInstigator->GetPawn());
	if (Character)
	{
		CombatTarget = Character;
		AggroTargets.Empty();
		DisplayDamageWidget(DamageAmount);

		if (IsOutsideAttackRadius())
		{
			UpdateEnemyState(EEnemyState::EES_Chasing);
		}
		else
		{
			UpdateEnemyState(EEnemyState::EES_Attacking);
		}
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

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, float DamageDealt, AActor* Hitter)
{
	if (!IsDead()) ShowHealthBar();
	Super::GetHit_Implementation(ImpactPoint, DamageDealt, Hitter);

	if (ShouldHitReact(DamageDealt, Attributes->GetHealth()))
	{
		ClearAttackTimer();
		StopAttackMontage();
		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
		SetExtraWeaponCollisionDisabled();
	}
}

//protected
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	EnemyController = Cast<AAIController>(GetController());

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemy::OnPerception);

	EnemyController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AEnemy::MoveCompleted);

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAggroSphereOverlap);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAggroSphereEndOverlap);

	AggroRadius = AggroSphere->GetScaledSphereRadius();

	HideHealthBar();
	SetCombatIndicatorWidgetState(0);
	MoveToTarget(PatrolTarget);
	SpawnDefaultWeapons();
	EventsSubsystem = GetGameInstance()->GetSubsystem<UEventsSubsystem>();
}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(false);

	UpdateEnemyState(EEnemyState::EES_Dead);
	ClearAttackTimer();
	HideHealthBar();
	SetCombatIndicatorWidgetState(0);
	DisableCapsule();
	SetLifeSpan(5.f);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SetExtraWeaponCollisionDisabled();
	SpawnSoul();
	if (EventsSubsystem) EventsSubsystem->CreateEvent(EObjectiveType::EOT_EnemyKilled, EnemyID, 1);
}

void AEnemy::Attack()
{
	Super::Attack();

	if (CombatTarget == nullptr) return;
	UpdateEnemyState(EEnemyState::EES_Engaged);
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	return !IsOutsideAttackRadius() && (!IsAttacking() || !GetWorldTimerManager().IsTimerActive(AttackTimer)) && !IsDead() && !IsEngaged();
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
	UpdateEnemyState(EEnemyState::EES_NoState);
}

void AEnemy::StartPatrolling()
{
	UpdateEnemyState(EEnemyState::EES_Patrolling);
}

void AEnemy::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
	// Don't interupt the enemy if they're already fighting
	// A new alert will interupt an old one
	if (IsInCombat() || IsDead()) return;

	ABaseCharacter* Char = Cast<ABaseCharacter>(Actor);
	
	if (Char && Stimulus.WasSuccessfullySensed())
	{
		AlertingLocation = Stimulus.StimulusLocation;
		UpdateEnemyState(EEnemyState::EES_Alerting);
	}
}

/*
 0   = directly in front and numbers increase clockwise:
 90  = directly to the right
 -90 = directly to the left
 180 / -180 is directly behind
 Check DirectionalHitReact function for more info
*/
float AEnemy::GetForwardAngleToTarget(AActor* Target)
{

	FVector ActorLoc = Target->GetActorLocation();
	const FVector ImpactLowered(ActorLoc.X, ActorLoc.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = |Forward||ToHit| * cost(theta)
	// |Forward| == 1 && |ToHit| == 1, so Forward * ToHit = cos(theta)
	double Theta = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ToHit)));

	// If crossproduct points down, theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(GetActorForwardVector(), ToHit);
	if (CrossProduct.Z < 0) Theta *= -1.f;

	return Theta;
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

void AEnemy::OnAggroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/* First check if the enemy should aggro on the target; return if no */
	if (Tags.Num() == 0 || OtherActor->ActorHasTag(Tags[0])) return;
	if (CombatTarget) return; /* Don't bother aggroing on a new enemy if they are currently engaged with one */
	if (EnemyState == EEnemyState::EES_Dead) return; /* Don't execute this code if the enemy is dead */

	/* Don't aggro on actors that aren't BaseCharacters */
	ABaseCharacter* Target = Cast<ABaseCharacter>(OtherActor);
	if (!Target) return;

	float CurrentAngle = GetForwardAngleToTarget(OtherActor);
	//UE_LOG(LogTemp, Warning, TEXT("OnAggroSphereOverlap(): %f"), CurrentAngle);

	if (CurrentAngle >= AggroAngleLeft && CurrentAngle <= AggroAngleRight)
	{
		CombatTarget = Target;
		//UE_LOG(LogTemp, Warning, TEXT("OnAggroSphereOverlap, emptying array: %s: %s"), *GetName(), *OtherActor->GetName());
		UpdateEnemyState(EEnemyState::EES_Chasing);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("OnAggroSphereOverlap, adding new element %s: %s"), *GetName(), *OtherActor->GetName());
		AggroTargets.AddUnique(Target);
	}
}

void AEnemy::OnAggroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABaseCharacter* Target = Cast<ABaseCharacter>(OtherActor);
	if (!Target) return;

	ABaseCharacter* ToRemove = nullptr;
	for (auto AggroTarget : AggroTargets)
	{
		if (AggroTarget == OtherActor)
		{
			ToRemove = AggroTarget;
		}
	}

	if (ToRemove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ONAggroSphereENDOverlap, removing element: %s: %s"), *GetName(), *OtherActor->GetName());
		AggroTargets.Remove(ToRemove);
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
		// Character / other enemy has been spotted but is not within this Enemy's combat radius
		//if (CombatTarget != nullptr) UE_LOG(LogTemp, Warning, TEXT("In CheckCombatTarget(): IsOutsideCombatRadius() %s: %s, %f, %f"), *GetName(), *CombatTarget->GetName(), AggroRadius, (CombatTarget->GetActorLocation() - GetActorLocation()).Size());
		ClearAttackTimer();
		
		if (IsInCombat() && CombatTarget != nullptr)
		{
			AlertingLocation = CombatTarget->GetActorLocation();
			UpdateEnemyState(EEnemyState::EES_Alerting);
		}
		if (!IsEngaged() && !IsAlerting())
		{
			//UE_LOG(LogTemp, Warning, TEXT("CheckCombatTarget(): About to set state to Patrolling"));
			UpdateEnemyState(EEnemyState::EES_Patrolling);
		}
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		// Enemy is probably Attacking or Engaging and and Character has moved outside of Attack Radius
		//UE_LOG(LogTemp, Warning, TEXT("In CheckCombatTarget(): IsOutsideAttackRadius() && !IsChasing() %s: %s"), *GetName(), *CombatTarget->GetName());
		ClearAttackTimer();
		if (!IsEngaged()) UpdateEnemyState(EEnemyState::EES_Chasing);
	}
	else if (CanAttack())
	{
		// Enemy is inside AttackRadius, Enemy may be Patrolling and turned away
		//UE_LOG(LogTemp, Warning, TEXT("In CheckCombatTarget(): Attacking %s: %s"), *GetName(), *CombatTarget->GetName());
		UpdateEnemyState(EEnemyState::EES_Attacking);
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

bool AEnemy::IsOutsideCombatRadius()
{
	//UE_LOG(LogTemp, Warning, TEXT("DistanceRadius, CombatRadius: %d, %d"), (CombatTarget->GetActorLocation() - GetActorLocation()).Size(), AggroRadius);
	return !InTargetRange(CombatTarget, AggroRadius + OverlapLeniency);
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

bool AEnemy::IsAlerting()
{
	return EnemyState == EEnemyState::EES_Alerting;
}

bool AEnemy::IsInCombat()
{
	return IsAttacking() || IsEngaged() || IsChasing();
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::ClearAlertTimer()
{
	GetWorldTimerManager().ClearTimer(AlertTimer);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	//UE_LOG(LogTemp, Warning, TEXT("InTargetRange(): Size: %f <= %f"), (Target->GetActorLocation() - GetActorLocation()).Size(), Radius);
	return (Target->GetActorLocation() - GetActorLocation()).Size() <= Radius;
}

void AEnemy::MoveToTarget(AActor* TheTarget, double Distance, FColor DebugColor)
{
	if (EnemyController == nullptr || TheTarget == nullptr) return;

	FAIMoveRequest MoveRequest;
	FNavPathSharedPtr NavPath;

	double AcceptanceDistance = Distance == 0.f ? AcceptanceRadius : Distance;

	MoveRequest.SetGoalActor(TheTarget);
	MoveRequest.SetAcceptanceRadius(AcceptanceDistance);

	EnemyController->MoveTo(MoveRequest, &NavPath);
	//DrawDebugSphere(GetWorld(), TheTarget->GetActorLocation(), 10.f, 8, FColor::Red, false, 60.f, 0, 2.f);
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

/*
* 0 = clear the state (hide the widget)
* 1 = alerting state
* 2 = combat state
*/
void AEnemy::SetCombatIndicatorWidgetState(int32 CombatState)
{
	if (CombatStateWidget)
	{
		CombatStateWidget->SetState(CombatState);
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

void AEnemy::UpdateEnemyState(EEnemyState State)
{
	if (EnemyState == EEnemyState::EES_Alerting && State != EEnemyState::EES_Alerting) ClearAlertTimer();
	if (EnemyState == EEnemyState::EES_Patrolling && State != EEnemyState::EES_Patrolling)
	{
		ClearPatrolTimer();
		EnemyController->StopMovement();
	}

	EnemyState = State;

	switch (State)
	{
	case EEnemyState::EES_NoState:
		//UE_LOG(LogTemp, Warning, TEXT("UpdateEnemyState(): Setting state to NoState"));
		CheckCombatTarget();
		break;
	case EEnemyState::EES_Patrolling:
		//UE_LOG(LogTemp, Warning, TEXT("UpdateEnemyState(): Setting state to Patrolling"));
		CombatTarget = nullptr;
		HideHealthBar();
		SetCombatIndicatorWidgetState(0);
		GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
		MoveToTarget(PatrolTarget);
		break;
	case EEnemyState::EES_Chasing:
		//UE_LOG(LogTemp, Warning, TEXT("UpdateEnemyState(): %s: Setting state to Chasing"), *GetName());
		GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
		ShowHealthBar();
		SetCombatIndicatorWidgetState(2);
		AggroTargets.Empty();
		MoveToTarget(CombatTarget);
		break;
	case EEnemyState::EES_Attacking:
		//UE_LOG(LogTemp, Warning, TEXT("UpdateEnemyState(): %s: Setting state to Attacking"), *GetName());
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, FMath::RandRange(AttackMin, AttackMax));
		break;
	case EEnemyState::EES_Alerting:
		//UE_LOG(LogTemp, Warning, TEXT("UpdateEnemyState(): %s: Setting state to Alerting"), *GetName());
		CombatTarget = nullptr;
		ClearAlertTimer();
		SetCombatIndicatorWidgetState(1);
		FindNavigablePath(AlertingLocation);
		break;
	}
}

bool AEnemy::FindNavigablePath(FVector DesiredLocation)
{
	// Will need to have error handling in the case that the DesiredLocation is unreachable
	if (EnemyController == nullptr) return false;
	//UE_LOG(LogTemp, Warning, TEXT("FindNavigablePath(): Moving to the AlertLocation"));

	FAIMoveRequest MoveRequest;
	FNavPathSharedPtr NavPath;

	MoveRequest.SetGoalLocation(DesiredLocation);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

	EnemyController->MoveTo(MoveRequest, &NavPath);
	//DrawDebugSphere(GetWorld(), DesiredLocation, 10.f, 8, FColor::Purple, false, 60.f, 0, 2.f);
	return true;
}

void AEnemy::MoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	// Prevent an accidental completed move execution while patrolling
	if (EnemyState != EEnemyState::EES_Alerting) return;
	//UE_LOG(LogTemp, Warning, TEXT("MoveCompleted(): Setting timer before going back to Patrolling"));

	GetWorldTimerManager().SetTimer(AlertTimer, this, &AEnemy::StartPatrolling, AlertWaitTime);
}
