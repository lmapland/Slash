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
#include "Items/Weapon.h"
#include "Items/LandscapeResource.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/Interactable.h"

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

	/* Character Mesh Components */
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	EyesComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Eyes"));
	EyesComponent->SetupAttachment(GetMesh());

	FaceComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face"));
	FaceComponent->SetupAttachment(GetMesh());

	NeckComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Neck"));
	NeckComponent->SetupAttachment(GetMesh());

	ChestComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest"));
	ChestComponent->SetupAttachment(GetMesh());

	ArmsComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	ArmsComponent->SetupAttachment(GetMesh());

	HandsComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands"));
	HandsComponent->SetupAttachment(GetMesh());

	LegsComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
	LegsComponent->SetupAttachment(GetMesh());

	FeetComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feet"));
	FeetComponent->SetupAttachment(GetMesh());

	EyebrowsComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EyebrowsMesh"));
	EyebrowsComponent->SetupAttachment(GetMesh());

	BeardComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Beard"));
	BeardComponent->SetupAttachment(GetMesh());

	MoustacheComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Moustache"));
	MoustacheComponent->SetupAttachment(GetMesh());

	SideburnComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sideburn"));
	SideburnComponent->SetupAttachment(GetMesh());

	HairComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HairMesh"));
	HairComponent->SetupAttachment(GetMesh());

	TopknotComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Topknot"));
	TopknotComponent->SetupAttachment(GetMesh());

	LegArmourComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegArmour"));
	LegArmourComponent->SetupAttachment(GetMesh());

	BootsArmourComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BootsArmour"));
	BootsArmourComponent->SetupAttachment(GetMesh());

	BeltArmourComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BeltArmour"));
	BeltArmourComponent->SetupAttachment(GetMesh());

	BracersArmourComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BracersArmour"));
	BracersArmourComponent->SetupAttachment(GetMesh());

	BackArmourComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackArmour"));
	BackArmourComponent->SetupAttachment(GetMesh());

	GloveArmourComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GloveArmour"));
	GloveArmourComponent->SetupAttachment(GetMesh());

	ChestArmourComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestArmour"));
	ChestArmourComponent->SetupAttachment(GetMesh());

	ShoulderArmourComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShoulderArmour"));
	ShoulderArmourComponent->SetupAttachment(GetMesh());

	HelmetArmourComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetArmour"));
	HelmetArmourComponent->SetupAttachment(GetMesh());



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
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ASlashCharacter::Interact);
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

void ASlashCharacter::SetOverlappingResource(ALandscapeResource* Resource)
{
	OverlappingResource = Resource;
}

void ASlashCharacter::AddItem(int ItemID, int Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("ItemID:Amount %i, %i "), ItemID, Amount);

	/* How the code will eventually go; though I might actually call AddItem() and have that execute this code
	if (Inventory)
	{
		int32 Overflow = Inventory.AddLenient(ItemID->GetItemID(), ItemID->GetAmount());
		if (Overflow > 0)
		{
			// spawn via class ref & drop amount on ground
		}
	}*/
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
		AItem* Item = Cast<AItem>(OverlappingItem);
		if (Item)
		{
			// TODO delete this: overlapping with a weapon should put it into the inventory just like all other items
			// keeping it for now, though, until I actually get the inventory working & also using items
			AWeapon* Weapon = Cast<AWeapon>(OverlappingItem);
			if (Weapon)
			{
				if (EquippedWeapon)
				{
					EquippedWeapon->Destroy();
				}
				Equip1HWeapon(Weapon);
			}
			else
			{
				Item->PickUp();
				AddItem(Item->GetItemID(), Item->GetAmount());
			}
		}
	}
	else if (OverlappingResource)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && GatheringMontage && OverlappingResource->Pickable())
		{
			UpdateActionState(EActionState::EAS_PickingUp);
			OverlappingResource->Pick();
			AnimInstance->Montage_Play(GatheringMontage, OverlappingResource->AnimSpeed);
			AnimInstance->Montage_JumpToSection(OverlappingResource->AnimationToPlay, GatheringMontage);
		}
	}
	else // is user trying to interact with an NPC?
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		// ObjectTypeQuery3 is 'Pawn' - if I expand this system to line trace to look for items as well (AActors) this will need to change
		ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery3);
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);
		FHitResult HitResult;

		// how to get the camera location & rotation in world space
		FVector MouseOrigin = ViewCamera->GetComponentLocation();
		FRotator MouseRotation = ViewCamera->GetComponentRotation();
		FVector DirectionOfLineTrace =  MouseRotation.Vector() * (InteractDistance + CameraBoom->TargetArmLength);

		UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), MouseOrigin, MouseOrigin + DirectionOfLineTrace, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
		//UKismetSystemLibrary::DrawDebugLine(this, MouseOrigin, MouseOrigin + DirectionOfLineTrace, FLinearColor::Black, 180.f);

		// LineTrace hit nothing. Do nothing.
		if (!HitResult.bBlockingHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("In Interact(): Performed line trace & hit nothing"));
			return;
		}

		if (SlashOverlay && Attributes)
		{
			UE_LOG(LogTemp, Warning, TEXT("In Interact(): Performed line trace & hit something: %s"), *HitResult.GetActor()->GetName());
			if (IInteractable* Interactable = Cast<IInteractable>(HitResult.GetActor()))
			{
				Interactable->Interact(SlashOverlay, Attributes);
			}
		}
	}
}

void ASlashCharacter::Dodge()
{
	if (IsOccupied()) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeMontage && SlashOverlay && Attributes && Attributes->UseStaminaIfAble(DodgeCost))
	{
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		AnimInstance->Montage_Play(DodgeMontage, 1.4f);

		FName SectionName = FName("Dodge");
		UpdateActionState(EActionState::EAS_Dodging);
		AnimInstance->Montage_JumpToSection(SectionName, DodgeMontage);
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
	if (ActionState == EActionState::EAS_Unoccupied && EquipMontage && EquippedWeapon)
	{
		if (bIsArmed) // could also check if CharacterState != ECS_Unequipped && EquippedWeapon
		{
			// put weapon away
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				UE_LOG(LogTemp, Warning, TEXT("Character is armed; playing Disarm animation"));
				AnimInstance->Montage_Play(EquipMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Disarm"), EquipMontage);
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
				AnimInstance->Montage_Play(EquipMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Arm"), EquipMontage);
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

void ASlashCharacter::ApplyPurchase(TArray<int> ItemsToRemove, TArray<int> AmountsToRemove, TArray<int> ItemsToAdd, TArray<int> AmountsToAdd, int Quantity)
{

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
