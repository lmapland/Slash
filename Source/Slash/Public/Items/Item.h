// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Item.generated.h"


enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class SLASH_API AItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;
	void PickedUp(); // finalize the pickup (destroy this object)
	void PickUp(); // Pick the object, causing it to be destroyed
	virtual void Interact(USlashOverlay* Overlay, UAttributeComponent* Attributes) override;
	virtual FString GetActorName() override;
	virtual FString GetInteractWord() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	virtual void SpawnPickupSystem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Hovering")
	float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Hovering")
	float TimeConstant = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* Sphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	class UNiagaraComponent* ItemParticles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FString ItemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 Amount = 1;

	UPROPERTY(EditAnywhere, Category = "Item Properties")
	class USoundBase* PickupSound;

	EItemState ItemState = EItemState::EIS_Equipped;

private:
	void PlayPickupSound();

	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;

	FTimerHandle DestroySelfTimer;

	// added when the Soul was added to be initialized and played when the Soul is overlapped with
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickupEffect;

public:
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return ItemMesh; }
	FORCEINLINE int32 GetAmount() const { return Amount; }
	FORCEINLINE int32 GetItemID() const { return ItemID; }
	FORCEINLINE void SetAmount(int32 Value) { Amount = Value; } // should probably have bounds checking TODO
};
