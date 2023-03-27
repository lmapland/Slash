// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuestComponent.h"
#include "Subsystems/EventsSubsystem.h"

UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Set up the tutorial quest
	// TODO change this to read from a DB or something
	Quests.Add(new FQuest);
	Quests[0]->DisplayName = TEXT("Tutorial Quest");
	
	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[0]->DisplayName = TEXT("Find and pick up the Beginner's Sword");
	Quests[0]->Objectives[0]->Type = EObjectiveType::EOT_ItemAdded;
	Quests[0]->Objectives[0]->ID = 10;

	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[1]->DisplayName = TEXT("Open your inventory and equip the sword");
	Quests[0]->Objectives[1]->Type = EObjectiveType::EOT_ItemEquipped;
	Quests[0]->Objectives[1]->ID = 10;
	Quests[0]->Objectives[0]->NextObjective = Quests[0]->Objectives[1];

	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[2]->DisplayName = TEXT("Kill two enemies");
	Quests[0]->Objectives[2]->Type = EObjectiveType::EOT_EnemyKilled;
	Quests[0]->Objectives[2]->NumEvents = 2;
	Quests[0]->Objectives[1]->NextObjective = Quests[0]->Objectives[2];

	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[3]->DisplayName = TEXT("Open your inventory and level up");
	Quests[0]->Objectives[3]->Type = EObjectiveType::EOT_LeveledUp;
	Quests[0]->Objectives[2]->NextObjective = Quests[0]->Objectives[3];

	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[4]->DisplayName = TEXT("Explore and find materials, then sell them to the Merchant");
	Quests[0]->Objectives[4]->Type = EObjectiveType::EOT_AttributeUpdated;
	Quests[0]->Objectives[4]->ID = 3; // gold
	Quests[0]->Objectives[4]->Amount = 1; // if the user gained at least 1 gold then they must have sold something to the merchant
	Quests[0]->Objectives[3]->NextObjective = Quests[0]->Objectives[4];

	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[5]->DisplayName = TEXT("Get 200 Gold");
	Quests[0]->Objectives[5]->Type = EObjectiveType::EOT_AttributeUpdated;
	Quests[0]->Objectives[5]->ID = 3;
	Quests[0]->Objectives[5]->Amount = 200;
	Quests[0]->Objectives[4]->NextObjective = Quests[0]->Objectives[5];

	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[6]->DisplayName = TEXT("Purchase the DoomSlayer");
	Quests[0]->Objectives[6]->Type = EObjectiveType::EOT_ItemAdded;
	Quests[0]->Objectives[6]->ID = 11;
	Quests[0]->Objectives[5]->NextObjective = Quests[0]->Objectives[6];

	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[7]->DisplayName = TEXT("Kill the boss");
	Quests[0]->Objectives[7]->Type = EObjectiveType::EOT_EnemyKilled;
	Quests[0]->Objectives[7]->ID = 6; // EnemyID of the boss
	Quests[0]->Objectives[6]->NextObjective = Quests[0]->Objectives[7];

	Quests[0]->CurrentObjective = Quests[0]->Objectives[0];
}

void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();

	UEventsSubsystem* EventsSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UEventsSubsystem>();
	EventsSubsystem->OnEventCreatedDelegate.AddUniqueDynamic(this, &UQuestComponent::OnQuestEventCreated);
}

void UQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UQuestComponent::OnQuestEventCreated(EObjectiveType ObjectiveType, int32 ID, int32 Amount)
{
	;
	UE_LOG(LogTemp, Warning, TEXT("In UQuestComponent::OnQuestEventCreated(): ID: %i, Amount: %i, %s"), ID, Amount, *GetOwner()->GetName());
}