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
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class APickup;
class UParticleSystem;
class APlayerController;
class Soul;
class ATreasure;
class AFlower;
class AMiningRock;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;
	virtual void AddHealth(AHealthPotion* HealthPot) override;
	virtual void AddStamina(AStaminaPotion* StamPot) override;

protected:
	virtual void BeginPlay() override;

	/** Functions for callbacks	*/
	void Move(const FInputActionValue& Value); // handles forward, back, and side-to-side
	void Look(const FInputActionValue& Value);
	void Interact();
	void Dodge();
	virtual void Attack() override;
	virtual void DodgeEnd() override;
	virtual int32 PlayAttackMontage() override;
	void Equip();
	void Block();
	void EndBlock();
	void ZoomIn();
	void ZoomOut();
	
	void Equip1HWeapon(AWeapon* Weapon);
	virtual bool CanAttack() override;
	virtual void Die_Implementation() override;
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void CompleteItemPickup();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();
	

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
	UInputAction* ZoomOutAction; // mouse wheel scroll up

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BlockAction; // left-click


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

private:
	void InitializeSlashOverlay();
	void SetHUDHealth();
	bool IsOccupied();
	/* Pickup items */
	void RemovePickedUpItem();

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere)
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	FTimerHandle DestroyPickupTimer;
	FTimerHandle InitilizationTimer;

	APickup* ItemPickedUp;

	bool bIsArmed; // Character currently has a weapon and it is in their hand

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* WeaponTrail;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
