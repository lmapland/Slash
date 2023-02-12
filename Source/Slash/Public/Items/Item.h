// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"


enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	template<typename T>
	T Avg(T First, T Second);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FName AnimationToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	class UNiagaraComponent* ItemParticles;

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
	
	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	FName ItemName;

	EItemState ItemState = EItemState::EIS_Equipped;

private:
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;
	
	// added when the Soul was added to be initialized and played when the Soul is overlapped with
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickupEffect;

public:
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return ItemMesh; }
	FORCEINLINE FName GetItemName() const { return ItemName; }
};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}

