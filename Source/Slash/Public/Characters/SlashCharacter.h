// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"
//#include <Slash/Public/Characters/SlashCharacter.h>

class USlashOverlay;
class UInputComponent;
class AController;
class UEventsSubsystem;
class UQuestComponent;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UParticleSystem;
class APlayerController;
class ALandscapeResource;
class USoundBase;
class UNiagaraSystem;
class AEnemy;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, float DamageDealt, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void AddItem(int ItemID, int Amount) override;
	void RemoveItem(int ItemID, int Amount);
	void UseItem(int32 ItemID, int32 InventorySlot, int32 Amount = 1);
	void AddToTargetList(AEnemy* ToAdd);
	void RemoveFromTargetList(AEnemy* ToRemove);

	UFUNCTION(BlueprintCallable)
	void DropItem(int32 SlotID, bool RemoveFromInventory = false);

	UFUNCTION()
	virtual void OnItemSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnItemSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	virtual void BeginPlay() override;

	/** Functions for callbacks	*/
	void Move(const FInputActionValue& Value); // handles forward, back, and side-to-side
	void Look(const FInputActionValue& Value);
	void Interact();
	void Dodge();
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual int32 PlayAttackMontage() override;
	void Equip();
	void Block();
	void EndBlock();
	void ZoomIn();
	void ZoomOut();
	void Sprint();
	void EndSprint();
	void OpenEverythingMenu();
	
	void Equip1HWeapon(AWeapon* Weapon);
	virtual bool CanAttack() override;
	bool CanSprint();
	bool CanInteract();
	virtual void Die_Implementation() override;
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void CompleteItemPickup();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UFUNCTION(BlueprintCallable)
	void RequestLevelUp();

	// false when purchase cannot be applied; true when it can
	UFUNCTION(BlueprintCallable)
	bool ApplyPurchase(TArray<int> ItemsToRemove, TArray<int> AmountsToRemove, TArray<int> ItemsToAdd, TArray<int> AmountsToAdd);
	
	UFUNCTION(BlueprintCallable)
	void FinishUseItem();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* ItemSphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UQuestComponent* Quests;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float InteractDistance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float TargetArmLength = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* CharMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction; // mousemove

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction; // e

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction; // spacebar

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DodgeAction; // z

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction; // click

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* EquipAction; // r

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ZoomInAction; // mouse wheel scroll up

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction; // left shift

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ZoomOutAction; // mouse wheel scroll up

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BlockAction; // left-click
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* OpenEverythingMenuAction; // left-click


	/* Zooming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float MinZoomLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float MaxZoomLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float ZoomInOutAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 DodgeCost = 15;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 AttackCost = 10;

	/* Movement Speeds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeedNormal = 400.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeedSprint = 700.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeedBlock = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeedUseItem = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Up")
	USoundBase* LevelUpSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Up")
	UNiagaraSystem* LevelUpParticles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	FName MainMenuLevelName = TEXT("MainMenu");

	UPROPERTY(VisibleAnywhere)
	TArray<AEnemy*> CurrentTargetOf;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MouseSensitivityMultiplier = 0.5f;

private:
	void InitializeSlashOverlay();
	void SetHUDHealth();
	bool IsOccupied();
	/* Pickup items */
	void UpdateActionState(EActionState State);
	AWeapon* SpawnWeapon(TSubclassOf<AItem> ItemRef);
	AItem* SpawnItem(TSubclassOf<AItem> ItemRef);
	void TossItem(AItem* ItemToToss);
	void FinishDie();
	void InteractWithLandscapeResource(ALandscapeResource* Resource);
	void LineTraceForInteractable();
	bool PerformLineTrace(FHitResult& HitResult);

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	/* Character Mesh Components */
	UPROPERTY(VisibleAnywhere)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere)
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* EyesComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FaceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* NeckComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ChestComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ArmsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HandsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* LegsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FeetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* EyebrowsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BeardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* MoustacheComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SideburnComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HairComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TopknotComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* LegArmourComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BootsArmourComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BeltArmourComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BracersArmourComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BackArmourComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* GloveArmourComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ChestArmourComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ShoulderArmourComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HelmetArmourComponent;


	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> OverlappingActors;

	AActor* HoveredInteractable;

	FTimerHandle InitilizationTimer;
	FTimerHandle DeathTimer;

	float DeathTime = 5.f;

	bool bIsArmed; // Character currently has a weapon and it is in their hand

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* WeaponTrail;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

	UEventsSubsystem* EventsSubsystem;

	int32 ItemInUse;
	int32 ItemInUseAmount;
	TArray<FString> UseTypes;
	int32 SlotIndex;

	AItem* EquippedResourceTool = nullptr;


public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE USlashOverlay* GetSlashOverlay() const { return SlashOverlay; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE APlayerController* GetPlayerController() const { return PlayerController; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UInventory* GetInventory() const { return Inventory; }
};
