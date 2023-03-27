// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/EventsSubsystem.h"

void UEventsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UEventsSubsystem::CreateEvent(EObjectiveType ObjectiveType, int32 ID, int32 Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("UEventsSubsystem::CreateEvent() called"));
	OnEventCreatedDelegate.Broadcast(ObjectiveType, ID, Amount);
}
