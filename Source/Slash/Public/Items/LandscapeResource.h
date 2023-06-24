// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "LandscapeResource.generated.h"


class USphereComponent;
class UCapsuleComponent;

UCLASS()
class SLASH_API ALandscapeResource : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ALandscapeResource();
	void PickedUp(); // finalize the pickup (destroy this object)
	void Pick(); // Pick the object, causing it to not be pickable anymore
	virtual void Interact(USlashOverlay* Overlay, UAttributeComponent* Attributes) override;
	virtual FString GetActorName() override;
	virtual FString GetInteractWord() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FName AnimationToPlay;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	float AnimSpeed = 1.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	// The tool, like a mining pick or an axe, to spawn for the animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 TooltoUse = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FName ToolSocket = FName("RightHandSocket");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 DropItem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 Amount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FString ResourceName = FString("Resource");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FString InteractWord = FString("Use");

private:
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	bool bPickable = true; // by default object can be picked

	FTimerHandle DestroySelfTimer;

public:
	FORCEINLINE bool Pickable() const { return bPickable; }
	FORCEINLINE int32 GetItemID() const { return DropItem; }
	FORCEINLINE int32 GetAmount() const { return Amount; }
	FORCEINLINE int32 GetToolID() const { return TooltoUse; }
	FORCEINLINE FName GetToolSocketName() const { return ToolSocket; }

};
