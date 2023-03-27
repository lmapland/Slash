// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/QuestObjectiveTypes.h"
#include "EventsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEventCreated, EObjectiveType, ObjectiveType, int32, ID, int32, Amount);

/**
 * 
 */
UCLASS()
class SLASH_API UEventsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void CreateEvent(EObjectiveType ObjectiveType, int32 ID, int32 Amount);

	UPROPERTY()
	FOnEventCreated OnEventCreatedDelegate;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
};
