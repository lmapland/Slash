// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Pickup.h"
#include "Items/Weapon.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/HealthPotion.h"
#include "Items/StaminaPotion.h"
#include "Items/Flower.h"
#include "Items/MiningRock.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"

// public
ASlashCharacter::ASlashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = TargetArmLength;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	MinZoomLength = 100.f;
	MaxZoomLength = 600.f;
	ZoomInOutAmount = 30.f;

	bIsArmed = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeedNormal;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ASlashCharacter::Tick(float DeltaTime)
{
	if (Attributes && SlashOverlay)
	{
		// Stamina use / regen
		if (ActionState == EActionState::EAS_Sprinting)
		{
			if (Attributes->GetStamina() > 0)
			{
				Attributes->DrainStamina(DeltaTime);
				SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
			}
			else
			{
				UpdateActionState(EActionState::EAS_Unoccupied);
			}
		}
		else if (ActionState != EActionState::EAS_Dead && ActionState != EActionState::EAS_Dodging && ActionState != EActionState::EAS_Attacking)
		{
			Attributes->RegenStamina(DeltaTime);
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Interact);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Equip);
		EnhancedInputComponent->BindAction(OpenEverythingMenuAction, ETriggerEvent::Completed, this, &ASlashCharacter::OpenEverythingMenu);

		EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Triggered, this, &ASlashCharacter::ZoomIn);
		EnhancedInputComponent->BindAction(ZoomOutAction, ETriggerEvent::Triggered, this, &ASlashCharacter::ZoomOut);

		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &ASlashCharacter::Block);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &ASlashCharacter::EndBlock);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ASlashCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASlashCharacter::EndSprint);

	}
}

void ASlashCharacter::Jump()
{
	if (ActionState != EActionState::EAS_Unoccupied && ActionState != EActionState::EAS_Attacking) return;

	Super::Jump();
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealth() > 0.f)
	{
		UpdateActionState(EActionState::EAS_HitReaction);
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	CombatTarget = EventInstigator->GetPawn();

	return DamageAmount;
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSouls(Attributes->GetSouls());
		SlashOverlay->SetLevelInfo(Attributes->GetLevel(), Attributes->GetPercentToNextLevel());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void ASlashCharacter::AddHealth(AHealthPotion* HealthPot)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddHealth(HealthPot->GetAmount());
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void ASlashCharacter::AddStamina(AStaminaPotion* StamPot)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddStamina(StamPot->GetAmount());
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

// protected
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Group3"));
	PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(CharMappingContext, 0);
		}
	}

	GetWorldTimerManager().SetTimer(InitilizationTimer, this, &ASlashCharacter::InitializeSlashOverlay, 2.f);
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied && ActionState != EActionState::EAS_Blocking && ActionState != EActionState::EAS_Sprinting) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisValue.X);
	AddControllerPitchInput(LookAxisValue.Y);
}

// note that Stephen is using the phrase "equip" while I use "interact" - we both use the 'e' key
void ASlashCharacter::Interact()
{
	if (OverlappingItem)
	{
		AWeapon* Weapon = Cast <AWeapon>(OverlappingItem);
		if (Weapon)
		{
			if (EquippedWeapon)
			{
				EquippedWeapon->Destroy();
			}
			Equip1HWeapon(Weapon); // need to handle 2-handers eventually here
		}
		else
		{
			APickup* ToPickUp = Cast<APickup>(OverlappingItem);
			if (ToPickUp)
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance && AttackMontage && ToPickUp->Pickable())
				{
					UpdateActionState(EActionState::EAS_PickingUp);
					ToPickUp->Pick();
					AnimInstance->Montage_Play(AttackMontage, ToPickUp->AnimSpeed);
					AnimInstance->Montage_JumpToSection(ToPickUp->AnimationToPlay, AttackMontage);
					
					ItemPickedUp = ToPickUp;
					GetWorldTimerManager().SetTimer(DestroyPickupTimer, this, &ASlashCharacter::RemovePickedUpItem, 2.f);

					/* I would love to move in this direction
					if (Attributes && SlashOverlay)
					{
						ItemPickedUp->UpdateAttributes(Attributes);
						ItemPickedUp->UpdateOverlay(SlashOverlay);
					}*/

					if (ItemPickedUp->GetItemName() == FName("Flower"))
					{
						if (Attributes && SlashOverlay)
						{
							Attributes->AddFlowers(ItemPickedUp->GetAmount());
							SlashOverlay->SetFlowers(Attributes->GetFlowers());
						}
					}
					else if (ItemPickedUp->GetItemName() == FName("Ore"))
					{
						if (Attributes && SlashOverlay)
						{
							Attributes->AddOre(ItemPickedUp->GetAmount());
							SlashOverlay->SetOre(Attributes->GetOre());
						}
					}
				}
			}
		}
	}
}

void ASlashCharacter::Dodge()
{
	if (IsOccupied()) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage && SlashOverlay && Attributes && Attributes->UseStaminaIfAble(DodgeCost))
	{
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		AnimInstance->Montage_Play(AttackMontage, 1.4f);

		FName SectionName = FName("Dodge");
		UpdateActionState(EActionState::EAS_Dodging);
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void ASlashCharacter::Attack()
{
	Super::Attack();

	if (CanAttack() && Attributes && Attributes->UseStaminaIfAble(AttackCost))
	{
		UpdateActionState(EActionState::EAS_Attacking);
		PlayAttackMontage();
	}
}

void ASlashCharacter::AttackEnd()
{
	UpdateActionState(EActionState::EAS_Unoccupied);
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	UpdateActionState(EActionState::EAS_Unoccupied);
}

int32 ASlashCharacter::PlayAttackMontage()
{
	Super::PlayAttackMontage();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, 1.f);

		FName SectionName = FName("Attack1");

		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
	return 0;
}

// "Arm" or "Disarm" depending on the current status
void ASlashCharacter::Equip() // r
{
	if (ActionState == EActionState::EAS_Unoccupied && AttackMontage)
	{
		if (bIsArmed) // could also check if CharacterState != ECS_Unequipped && EquippedWeapon
		{
			// put weapon away
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				UE_LOG(LogTemp, Warning, TEXT("Character is armed; playing Disarm animation"));
				AnimInstance->Montage_Play(AttackMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Disarm"), AttackMontage);
				UpdateActionState(EActionState::EAS_Equipping); // so the char can't move
			}

			CharacterState = ECharacterState::ECS_Unequipped;
			bIsArmed = false;
		}
		else
		{
			// get weapon out
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				UE_LOG(LogTemp, Warning, TEXT("Character is Not armed; playing Arm animation"));
				AnimInstance->Montage_Play(AttackMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Arm"), AttackMontage);
				UpdateActionState(EActionState::EAS_Equipping); // so the char can't move
			}

			if (EquippedWeapon)
			{
				CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon; // needs refactoring: what if toon has 2H weapon? Or bow or staff or whatever
			}
			bIsArmed = true;
		}
	}
}

void ASlashCharacter::Block()
{
	// user holds down right mouse button to block
	UpdateActionState(EActionState::EAS_Blocking);
}

void ASlashCharacter::EndBlock()
{
	// user holds down right mouse button to block
	UpdateActionState(EActionState::EAS_Unoccupied);
}

void ASlashCharacter::ZoomIn()
{
	if (CameraBoom->TargetArmLength > MinZoomLength)
	{
		CameraBoom->TargetArmLength -= ZoomInOutAmount;
	}
}

void ASlashCharacter::ZoomOut()
{
	if (CameraBoom->TargetArmLength <= MaxZoomLength)
	{
		CameraBoom->TargetArmLength += ZoomInOutAmount;
	}
}

void ASlashCharacter::Sprint()
{
	if (CanSprint()) UpdateActionState(EActionState::EAS_Sprinting);
}

void ASlashCharacter::EndSprint()
{
	if (ActionState == EActionState::EAS_Sprinting) UpdateActionState(EActionState::EAS_Unoccupied);
}

/*
* It may seem like this should be a toggle, but this code only ever opens the menu.
* In the WBP code I use "UI Mode Only" when this Tab/Everything menu is opened,
* therefore all further input is captured and handled by the WBP. The WBP is
* responsible for handling Hiding the menu, not this code.
 */
void ASlashCharacter::OpenEverythingMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("In OpenEverythingMenu()"));
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->OpenEverythingMenu(Attributes->GetSouls(), Attributes->GetSoulsUntilNextLevel(), Attributes->GetLevel() + 1);
	}
}

void ASlashCharacter::Equip1HWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	EquippedWeapon = Weapon;
	bIsArmed = true;
}

bool ASlashCharacter::CanAttack()
{
	return (ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped);
}

bool ASlashCharacter::CanSprint()
{
	return (ActionState == EActionState::EAS_Unoccupied) &&
		(GetVelocity().X != 0.f) && (GetVelocity().Y != 0.f) &&
		(Attributes->GetStamina() > 0);
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	UpdateActionState(EActionState::EAS_Dead);
	DisableMeshCollision();
}

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("In Disarm(): EquippedWeapon is true"));
	}
	if (CharacterState == ECharacterState::ECS_Unequipped)
	{
		UE_LOG(LogTemp, Warning, TEXT("In Disarm(): CharacterState is Unequipped"));
	}
	else if (CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("In Disarm(): CharacterState is EquippedOneHandedWeapon"));
	}

	if (EquippedWeapon) //&& CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
		UpdateActionState(EActionState::EAS_Unoccupied);
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	// if the character is 1H, pick up their single weapon
	if (CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("In Arm(): 1H Equip"));
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
	UpdateActionState(EActionState::EAS_Unoccupied);
}

void ASlashCharacter::CompleteItemPickup()
{
	// Called by Anim Notify to un-stuck the character
	UpdateActionState(EActionState::EAS_Unoccupied);
}

void ASlashCharacter::HitReactEnd()
{
	UpdateActionState(EActionState::EAS_Unoccupied);
}

void ASlashCharacter::RequestLevelUp()
{
	if (Attributes && SlashOverlay)
	{
		Attributes->LevelUp();
		SlashOverlay->SetLevelInfo(Attributes->GetLevel(), Attributes->GetPercentToNextLevel());
		SlashOverlay->UpdateTabWidget(Attributes->GetSouls(), Attributes->GetSoulsUntilNextLevel(), Attributes->GetLevel() + 1);
		UE_LOG(LogTemp, Warning, TEXT("Called UpdateTabWidget"));
	}
}

// private
void ASlashCharacter::InitializeSlashOverlay()
{
	if (PlayerController)
	{
		ASlashHUD* HUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if (HUD)
		{
			SlashOverlay = HUD->GetSlashOverlay();
			if (SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.f); // Attributes->GetStaminaPercent());
				SlashOverlay->SetGold(0);
				SlashOverlay->SetSouls(0);
				SlashOverlay->SetOre(0);
				SlashOverlay->SetFlowers(0);
				SlashOverlay->SetLevelInfo(Attributes->GetLevel(), Attributes->GetPercentToNextLevel());
			}
		}
	}
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

bool ASlashCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void ASlashCharacter::RemovePickedUpItem()
{
	ItemPickedUp->Destroy();
	ItemPickedUp = nullptr;
}

void ASlashCharacter::UpdateActionState(EActionState State)
{
	switch (State)
	{
	case EActionState::EAS_Attacking:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeedNormal;
		break;
	case EActionState::EAS_Blocking:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeedBlock;
		break;
	case EActionState::EAS_Dodging:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeedNormal;
		break;
	case EActionState::EAS_Equipping:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeedNormal;
		break;
	case EActionState::EAS_HitReaction:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeedNormal;
		break;
	case EActionState::EAS_PickingUp:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeedNormal;
		break;
	case EActionState::EAS_Unoccupied:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeedNormal;
		break;
	case EActionState::EAS_Sprinting:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeedSprint;
		break;
	};

	ActionState = State;
}
