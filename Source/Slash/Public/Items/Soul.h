// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Instant.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ASoul : public AInstant
{
	GENERATED_BODY()

public:
	ASoul();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 SoulCount = 1;

	double DesiredZ;

	UPROPERTY(EditAnywhere)
	float DriftRate = -15.f;

public:
	FORCEINLINE int32 GetSouls() const { return SoulCount; }
	FORCEINLINE void SetSouls(int32 Value) { SoulCount = Value; }
};
