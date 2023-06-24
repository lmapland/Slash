// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Animation/AnimInstance.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Subsystems/EventsSubsystem.h"
#include "Components/QuestComponent.h"
#include "Components/QuestObjectiveTypes.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/Inventory.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapon.h"
#include "Items/LandscapeResource.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Interfaces/Interactable.h"
#include "Enemy/Enemy.h"

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

	ItemSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemSphere"));
	ItemSphere->SetupAttachment(GetRootComponent());
	ItemSphere->SetSphereRadius(150.f);

	Quests = CreateDefaultSubobject<UQuestComponent>(TEXT("Quests"));

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
	UseTypes.Add("Direct");
	UseTypes.Add("Weapon");
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeedNormal;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ASlashCharacter::Tick(float DeltaTime)
{
	if (Attributes && SlashOverlay)
	{
		// Interactable Actor display
		if (OverlappingActors.Num() > 0) LineTraceForInteractable();

		// Health regen
		if (Attributes->GetHealth() < Attributes->GetMaxHealth())
		{
			Attributes->RegenHealth(DeltaTime);
			SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
		}

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
	if (ActionState != EActionState::EAS_Unoccupied && ActionState != EActionState::EAS_Attacking && ActionState != EActionState::EAS_Sprinting) return;

	Super::Jump();
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, float DamageDealt, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, DamageDealt, Hitter);

	if (Attributes && Attributes->GetHealth() > 0.f)
	{
		if (ShouldHitReact(DamageDealt, Attributes->GetHealth()))
		{
			SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
			UpdateActionState(EActionState::EAS_HitReaction);
		}
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	ABaseCharacter* Character = Cast<ABaseCharacter>(EventInstigator->GetPawn());
	if (Character)
	{
		CombatTarget = Character;
		DisplayDamageWidget(DamageAmount);
	}
	return DamageAmount;
}

void ASlashCharacter::AddItem(int ItemID, int Amount)
{
	if (Inventory && Attributes && SlashOverlay)
	{
		if (Inventory->IsAttribute(ItemID))
		{
			float ReturnedAttribute = Attributes->AddAttribute(ItemID, Amount);
			SlashOverlay->UpdateAttribute(ItemID, ReturnedAttribute);
			SlashOverlay->SetLevelInfo(Attributes->GetLevel(), Attributes->GetPercentToNextLevel());
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("ASlashCharacter::AddItem(): (AddLenient) - (ItemID,Amount): (%i,%i) "), ItemID, Amount);
			int32 OriginalAmount = Amount;
			TSubclassOf<AItem> ItemRef = Inventory->AddLenient(ItemID, Amount);
			if (Amount > 0)
			{
				AItem* Spawned = SpawnItem(ItemRef);
				Spawned->SetAmount(Amount);
				TossItem(Spawned);

				// Update the UI with info about which items were gathered and which were tossed away
				if (OriginalAmount == Amount) // added no items
				{
					SlashOverlay->UpdateItemPickupText(ItemID, 0); // 0 in Amount to indicate that there is no room in the inventory
				}
				else
				{
					SlashOverlay->UpdateItemPickupText(ItemID, OriginalAmount - Amount);
					if (EventsSubsystem) EventsSubsystem->CreateEvent(EObjectiveType::EOT_ItemAdded, ItemID, OriginalAmount - Amount);
				}
			}
			else
			{
				SlashOverlay->UpdateItemPickupText(ItemID, OriginalAmount);
				if (EventsSubsystem) EventsSubsystem->CreateEvent(EObjectiveType::EOT_ItemAdded, ItemID, OriginalAmount);
			}
		}
	}
}

// Expects a positive value in Amount
void ASlashCharacter::RemoveItem(int ItemID, int Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("In ASlashCharacter::RemoveItem():"));
	if (Inventory && Attributes && SlashOverlay)
	{
		if (Inventory->IsAttribute(ItemID))
		{
			float ReturnedAttribute = Attributes->AddAttribute(ItemID, Amount * -1);
			SlashOverlay->UpdateAttribute(ItemID, ReturnedAttribute);
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("ASlashCharacter::AddItem(): (AddLenient) - (ItemID,Amount): (%i,%i) "), ItemID, Amount);
			Inventory->RemoveItem(ItemID, Amount);
		}
	}
}

/*
* The thing to apply takes effect only when the animation is complete
*/
void ASlashCharacter::UseItem(int32 ItemID, int32 InventorySlot, int32 Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("In UseItem()"));
	ItemInUse = ItemID;
	ItemInUseAmount = Amount;
	SlotIndex = InventorySlot;

	// First need to know what the effects are: Get the DataTable element for this Name
	FItemStructure* ItemRow = InventoryDataTable->FindRow<FItemStructure>(FName(FString::FromInt(ItemInUse)), "InventoryItems");

	UpdateActionState(EActionState::EAS_UsingItem);

	if (ItemRow->UseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ItemRow->UseSound, GetActorLocation());
	}
	if (ItemRow->UseParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ItemRow->UseParticles, GetActorLocation());
	}

	// Play animation (add end of anim function to end their item use and remove them from that state) - Anim Montage & Anim Section Name from DT

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ItemUseMontage && !ItemRow->UseAnimSectionName.IsNone())
	{
		AnimInstance->Montage_Play(ItemUseMontage, 1.f);
		AnimInstance->Montage_JumpToSection(ItemRow->UseAnimSectionName, ItemUseMontage);
	}
	else
	{
		GetWorldTimerManager().SetTimer(InitilizationTimer, this, &ASlashCharacter::FinishUseItem, 2.f);
	}
}

void ASlashCharacter::AddToTargetList(AEnemy* ToAdd)
{
	CurrentTargetOf.AddUnique(ToAdd);

	if (CurrentTargetOf.Num() == 1)
	{
		SlashOverlay->SetCombatInstructionsVisible(true);
	}
}

void ASlashCharacter::RemoveFromTargetList(AEnemy* ToRemove)
{
	CurrentTargetOf.Remove(ToRemove);

	if (CurrentTargetOf.Num() == 0)
	{
		SlashOverlay->SetCombatInstructionsVisible(false);
	}
}

void ASlashCharacter::DropItem(int32 SlotID, bool RemoveFromInventory)
{
	FInventorySlot* Slot = Inventory->GetSlotRef(SlotID);
	FItemStructure* ItemStruct = Inventory->GetItemStructure(Slot->ItemID);
	
	if (ItemStruct)
	{
		AItem* Spawned = SpawnItem(ItemStruct->class_ref);
		Spawned->SetAmount(Slot->CurrentStack);
		TossItem(Spawned);
	}

	SlashOverlay->UpdateItemPickupText(Slot->ItemID, Slot->CurrentStack * -1);

	if (RemoveFromInventory)
	{
		Inventory->Empty(SlotID);
	}
}

void ASlashCharacter::OnItemSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractable* InteractableObject = Cast<IInteractable>(OtherActor);
	if (InteractableObject)
	{
		OverlappingActors.AddUnique(OtherActor);
	}
}

void ASlashCharacter::OnItemSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractable* InteractableObject = Cast<IInteractable>(OtherActor);
	if (InteractableObject)
	{
		OverlappingActors.Remove(OtherActor);
	}

	if (OverlappingActors.Num() == 0)
	{
		HoveredInteractable = nullptr;
		SlashOverlay->HideInteractMsg();
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

	ItemSphere->OnComponentBeginOverlap.AddDynamic(this, &ASlashCharacter::OnItemSphereOverlap);
	ItemSphere->OnComponentEndOverlap.AddDynamic(this, &ASlashCharacter::OnItemSphereEndOverlap);

	EventsSubsystem = GetGameInstance()->GetSubsystem<UEventsSubsystem>();

	Inventory->Setup(InventoryDataTable, 40, true);

	GetWorldTimerManager().SetTimer(InitilizationTimer, this, &ASlashCharacter::InitializeSlashOverlay, 2.f);
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied && ActionState != EActionState::EAS_Blocking && ActionState != EActionState::EAS_Sprinting && ActionState != EActionState::EAS_UsingItem) return;

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
	if (!CanInteract()) return;

	FHitResult HitResult;
	if (!PerformLineTrace(HitResult)) return;

	if (SlashOverlay && Attributes)
	{
		UE_LOG(LogTemp, Warning, TEXT("In Interact(): Performed line trace & hit something: %s"), *HitResult.GetActor()->GetName());
		if (IInteractable* Interactable = Cast<IInteractable>(HitResult.GetActor()))
		{
			// We're sure this item is interactable; now sort out what to do with it
			// Items
			AItem* Item = Cast<AItem>(Interactable);
			if (Item)
			{
				Item->PickUp();
				AddItem(Item->GetItemID(), Item->GetAmount());
				return;
			}

			// Landscape Resource
			ALandscapeResource* Resource = Cast<ALandscapeResource>(Interactable);
			if (Resource)
			{
				InteractWithLandscapeResource(Resource);
			}

			// Merchant & Containers
			Interactable->Interact(SlashOverlay, Attributes);
		}
	}
}

void ASlashCharacter::InteractWithLandscapeResource(ALandscapeResource* Resource)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && GatheringMontage && Resource->Pickable())
	{
		UpdateActionState(EActionState::EAS_PickingUp);
		int32 ToolID = Resource->GetToolID();
		FName SocketToUse = Resource->GetToolSocketName();
		if (ToolID > -1)
		{
			if (EquippedWeapon)
			{
				EquippedWeapon->GetMesh()->SetHiddenInGame(true);
			}

			// Get DT item
			FItemStructure* ItemRow = InventoryDataTable->FindRow<FItemStructure>(FName(FString::FromInt(ToolID)), "SpawnResourceTool");
			AItem* Spawned = SpawnItem(ItemRow->class_ref);
			EquippedResourceTool = Spawned;

			if (Spawned->GetMesh())
			{
				FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
				Spawned->GetMesh()->AttachToComponent(GetMesh(), TransformRules, SocketToUse);
			}
		}
		Resource->Pick();
		AddItem(Resource->GetItemID(), Resource->GetAmount());
		AnimInstance->Montage_Play(GatheringMontage, Resource->AnimSpeed);
		AnimInstance->Montage_JumpToSection(Resource->AnimationToPlay, GatheringMontage);
	}
}

void ASlashCharacter::LineTraceForInteractable()
{
	//UE_LOG(LogTemp, Warning, TEXT("LineTraceForInteractable()"));
	FHitResult HitResult;

	if (PerformLineTrace(HitResult))
	{
		IInteractable* Interactable = Cast<IInteractable>(HitResult.GetActor());
		if (Interactable)
		{
			if (HoveredInteractable)
			{
				// Hit Result && Hovered: either they are the same or different
				if (HitResult.GetActor() == HoveredInteractable) return;
				else
				{
					HoveredInteractable = HitResult.GetActor();
					SlashOverlay->UpdateInteractMsg(Interactable->GetActorName(), Interactable->GetInteractWord());
				}
			}
			else
			{
				// Hit Result && NO Hovered: set Hovered
				HoveredInteractable = HitResult.GetActor();
				SlashOverlay->UpdateInteractMsg(Interactable->GetActorName(), Interactable->GetInteractWord());
			}
		}
	}
	else
	{
		if (HoveredInteractable)
		{
			// NO Hit Result && Hovered: Unset Hovered
			HoveredInteractable = nullptr;
			SlashOverlay->HideInteractMsg();
		}
	}
}

bool ASlashCharacter::PerformLineTrace(FHitResult& HitResult)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	if (EquippedWeapon) ActorsToIgnore.Add(EquippedWeapon);

	// how to get the camera location & rotation in world space
	FVector MouseOrigin = ViewCamera->GetComponentLocation();
	FRotator MouseRotation = ViewCamera->GetComponentRotation();
	FVector DirectionOfLineTrace = MouseRotation.Vector() * (InteractDistance + CameraBoom->TargetArmLength);

	UKismetSystemLibrary::LineTraceSingle(GetWorld(), MouseOrigin, MouseOrigin + DirectionOfLineTrace, ETraceTypeQuery::TraceTypeQuery4, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	//UKismetSystemLibrary::DrawDebugLine(this, MouseOrigin, MouseOrigin + DirectionOfLineTrace, FLinearColor::Black, 180.f);

	if (!HitResult.bBlockingHit)
	{
		return false;
	}

	return true;
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
	EquippedWeapon->UpdateDamageModifier(Attributes->GetLevel() * 0.1f + 1.f);
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

bool ASlashCharacter::CanInteract()
{
	return (ActionState == EActionState::EAS_Unoccupied || ActionState == EActionState::EAS_Sprinting);
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	UpdateActionState(EActionState::EAS_Dead);
	DisableMeshCollision();
	if (SlashOverlay) SlashOverlay->ShowDeathWidget();
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.f, 1.f, DeathTime, FLinearColor::Black, false, true);
	GetWorldTimerManager().SetTimer(DeathTimer, this, &ASlashCharacter::FinishDie, DeathTime);
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

/* Called when the user completes the animaton for picking up a LandscapeResource */
void ASlashCharacter::CompleteItemPickup()
{
	UpdateActionState(EActionState::EAS_Unoccupied);

	if (EquippedResourceTool)
	{
		EquippedResourceTool->Destroy();
		EquippedResourceTool = nullptr;
	}

	if (EquippedWeapon && EquippedWeapon->GetMesh())
	{
		EquippedWeapon->GetMesh()->SetHiddenInGame(false);
	}
}

void ASlashCharacter::HitReactEnd()
{
	UpdateActionState(EActionState::EAS_Unoccupied);
}

void ASlashCharacter::RequestLevelUp()
{
	//UE_LOG(LogTemp, Warning, TEXT("In RequestLevelUp():"));
	if (Attributes && SlashOverlay)
	{
		if (Attributes->LevelUp())
		{
			UE_LOG(LogTemp, Warning, TEXT("RequestLevelUp(): GetLevel: %i, GetPercentToNextLevel: %f"), Attributes->GetLevel(), Attributes->GetPercentToNextLevel());
			SlashOverlay->SetLevelInfo(Attributes->GetLevel(), Attributes->GetPercentToNextLevel());
			SlashOverlay->UpdateTabWidget(Attributes->GetSouls(), Attributes->GetSoulsUntilNextLevel(), Attributes->GetLevel() + 1);
			if (EventsSubsystem) EventsSubsystem->CreateEvent(EObjectiveType::EOT_LeveledUp, Attributes->GetLevel(), -1);

			if (LevelUpSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), LevelUpSound);
			}
			if (LevelUpParticles)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LevelUpParticles, GetActorLocation());
			}

			if (EquippedWeapon)
			{
				EquippedWeapon->UpdateDamageModifier(Attributes->GetLevel() * 0.1f + 1.f);
			}
		}
	}
}

bool ASlashCharacter::ApplyPurchase(TArray<int> ItemsToRemove, TArray<int> AmountsToRemove, TArray<int> ItemsToAdd, TArray<int> AmountsToAdd)
{
	// check whether removal request can be completed: return false if not
	// TODO at the moment I am treating these arrays like single ints, but this can and should be changed in the future
	FItemStructure* ItemRow = InventoryDataTable->FindRow<FItemStructure>(FName(FString::FromInt(ItemsToRemove[0])), "InventoryItems");
	if (ItemRow->is_attribute)
	{
		if (!Attributes->Contains(ItemsToRemove[0], AmountsToRemove[0]))
		{
			UE_LOG(LogTemp, Warning, TEXT("ApplyPurchase(): (is_attribute) Inventory does not contain the necessary items to make the purchase"));
			return false;
		}
	}
	else if (!Inventory->Contains(ItemsToRemove[0], AmountsToRemove[0]))
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyPurchase(): Inventory does not contain the necessary items to make the purchase"));
		return false;
	}
	
	// check whether add request can be completed: return false if not
	if (!Inventory->HasSpace(ItemsToAdd[0], AmountsToAdd[0]))
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyPurchase(): Inventory cannot hold the requested Items To Add"));
		return false;
	}

	AddItem(ItemsToAdd[0], AmountsToAdd[0]);
	RemoveItem(ItemsToRemove[0], AmountsToRemove[0]);

	if (SlashOverlay)
	{
		SlashOverlay->UpdateItemPickupText(ItemsToAdd[0], AmountsToAdd[0]);
		SlashOverlay->UpdateGold(Attributes->GetGold());
	}

	return true;
}

void ASlashCharacter::FinishUseItem()
{
	FItemStructure* ItemRow = InventoryDataTable->FindRow<FItemStructure>(FName(FString::FromInt(ItemInUse)), "InventoryItems");
	
	switch (ItemRow->UseType)
	{
		case 0: // Direct, like potions
		{
			AddItem(ItemRow->UseItemID, ItemRow->UseAmount);
			if (EventsSubsystem) EventsSubsystem->CreateEvent(EObjectiveType::EOT_ItemUsed, ItemInUse, ItemInUseAmount);
			break;
		}
		case 1: // Weapon, for equipping
		{
			//UE_LOG(LogTemp, Warning, TEXT("FinishUseItem(): Equipping the weapon"));
			int32 OldWeaponID = 0;
			if (EquippedWeapon)
			{
				OldWeaponID = EquippedWeapon->GetItemID();
				//UE_LOG(LogTemp, Warning, TEXT("FinishUseItem(): There is an Equipped Weapon; storing the info. OldWeaponID: %i"), OldWeaponID);
				EquippedWeapon->Destroy();
			}

			// then spawn the new weapon
			AWeapon* Spawned = SpawnWeapon(ItemRow->class_ref);
			EquippedWeapon = Spawned;
			Equip1HWeapon(Spawned);
			if (EventsSubsystem) EventsSubsystem->CreateEvent(EObjectiveType::EOT_ItemEquipped, Spawned->GetItemID(), Spawned->GetAmount());

			if (OldWeaponID)
			{
				if (SlotIndex == -1) // weapon was right-clicked from a container
				{
					//UE_LOG(LogTemp, Warning, TEXT("FinishUseItem(): Equipping the weapon from a container"));
					int32 AmountNotAdded = 1;
					TSubclassOf<AItem> ItemRef = Inventory->AddLenient(OldWeaponID, AmountNotAdded);
					if (AmountNotAdded != 0) // inventory is full!
					{
						UE_LOG(LogTemp, Warning, TEXT("FinishUseItem(): No room left in user's inventory"));
						TossItem(SpawnWeapon(ItemRef));
					}
				}
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("FinishUseItem(): Equipping the weapon from user Inventory. SlotIndex: %i, OldWeaponID: %i"), SlotIndex, OldWeaponID);
					// Add old weapon to inventory
					Inventory->AddToSlot(SlotIndex, OldWeaponID, 1);
				}
				SlashOverlay->RefreshUserInventory();
			}

			break;
		}
		default:
			break;
	}

	ItemInUse = 0;
	ItemInUseAmount = 0;
	SlotIndex = 0;
	// Set the ActionState back to normal
	UpdateActionState(EActionState::EAS_Unoccupied);
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
				SlashOverlay->SetStaminaBarPercent(1.f);
				SlashOverlay->UpdateGold(Attributes->GetGold());
				SlashOverlay->SetLevelInfo(Attributes->GetLevel(), Attributes->GetPercentToNextLevel());
				if (Quests)
				{
					Quests->Setup(SlashOverlay->GetQuestWidget());
				}
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
		//UE_LOG(LogTemp, Warning, TEXT("EAS_Unoccupied: MaxWalkSpeed being updated to %f"), WalkSpeedNormal);
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeedNormal;
		break;
	case EActionState::EAS_Sprinting:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeedSprint;
		break;
	case EActionState::EAS_UsingItem:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeedUseItem;
		break;
	};

	ActionState = State;
}

AWeapon* ASlashCharacter::SpawnWeapon(TSubclassOf<AItem> ItemRef)
{
	FVector LocationToSpawn = GetActorLocation() + (GetActorForwardVector() * 40) + FVector(0.f, 0.f, 25.f);
	AWeapon* WepSpawned = GetWorld()->SpawnActor<AWeapon>(ItemRef, LocationToSpawn, GetActorRotation());

	return WepSpawned;
}

AItem* ASlashCharacter::SpawnItem(TSubclassOf<AItem> ItemRef)
{
	FVector LocationToSpawn = GetActorLocation() + (GetActorForwardVector() * 40) + FVector(0.f, 0.f, 25.f);
	AItem* ItemSpawned = GetWorld()->SpawnActor<AItem>(ItemRef, LocationToSpawn, GetActorRotation());

	return ItemSpawned;
}

void ASlashCharacter::TossItem(AItem* ItemToToss)
{
	ItemToToss->GetMesh()->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	ItemToToss->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemToToss->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	ItemToToss->GetMesh()->SetSimulatePhysics(true);
	ItemToToss->GetMesh()->AddImpulse(GetActorForwardVector() * 500 + FVector(0.f, 0.f, 250.f));
}

void ASlashCharacter::FinishDie()
{
	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}
