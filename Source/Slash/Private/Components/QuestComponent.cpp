// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuestComponent.h"
#include "Subsystems/EventsSubsystem.h"
#include "HUD/QuestsWidget.h"

UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Set up the tutorial quest
	// TODO change this to read from a DB or something
	Quests.Add(new FQuest);
	Quests[0]->DisplayName = TEXT("Tutorial Quest");
	
	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[0]->DisplayName = TEXT("Find and pick up the Beginner's Sword");
	Quests[0]->Objectives[0]->Type = EObjectiveType::EOT_ItemAdded;
	Quests[0]->Objectives[0]->ID = 10;

	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[1]->DisplayName = TEXT("Open your inventory with 'tab' and equip the sword");
	Quests[0]->Objectives[1]->Type = EObjectiveType::EOT_ItemEquipped;
	Quests[0]->Objectives[1]->ID = 10;
	Quests[0]->Objectives[0]->NextObjective = Quests[0]->Objectives[1];

	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[2]->DisplayName = TEXT("Kill two enemies (left click to swing)");
	Quests[0]->Objectives[2]->Type = EObjectiveType::EOT_EnemyKilled;
	Quests[0]->Objectives[2]->NumEvents = 2;
	Quests[0]->Objectives[1]->NextObjective = Quests[0]->Objectives[2];

	Quests[0]->Objectives.Add(new FObjective);
	Quests[0]->Objectives[3]->DisplayName = TEXT("Open your inventory with 'tab' and level up");
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
	Quests[0]->Objectives[6]->DisplayName = TEXT("Purchase the Doom Bringer");
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

/*
* A note on NumEvents vs Amount:
*  In general, NumEvents and Amounts are very similar. One could create a quest that asks for 5 mobs killed and they could set either NumEvents or Amount to 5
*   and the quest would complete identically.
*  BUT in the case where the dev has a quest for Attributes to be above a certain amount, NumEvents and Amount are NOT the same since Amount means something
*   different when talking in terms of Attributes vs Items (or Enemies). Because the TotalAmount is sent in for Attributes instead of the
*   GainedAmount, to test that Gold > 200 the dev cannot check for either (200 Events of 1 Gold) OR (1 event of 200 Gold). The dev must
*   specify that they are looking for 1 Event with Amount == 200.
*  The logic will treat NumEvents and Amount as the same in terms of Items, Item Use, Leveling Up, and Enemies, but different in terms of Attributes.
*  Except in the case where NumEvents > 1 && Amount > 1. In that case, the rules for all event types are the same.
*/
void UQuestComponent::OnQuestEventCreated(EObjectiveType ObjectiveType, int32 ID, int32 Amount)
{
	// We know there's only 1 quest at the moment
	// To add more quests we would need to do more work to add those quests, so I'm leaving this code to handle only a single quest
	if (Quests[0]->bComplete) return;
	if (Quests[0]->CurrentObjective->Type != ObjectiveType) return;
	//UE_LOG(LogTemp, Warning, TEXT("In UQuestComponent::OnQuestEventCreated(): Objective types MATCH"));
	if (Quests[0]->CurrentObjective->ID != -1 && Quests[0]->CurrentObjective->ID != ID) return;
	//UE_LOG(LogTemp, Warning, TEXT("In UQuestComponent::OnQuestEventCreated(): IDs MATCH"));

	//UE_LOG(LogTemp, Warning, TEXT("In UQuestComponent::OnQuestEventCreated(): Values passed in: ID: %i, Amount: %i"), ID, Amount);
	// 4 cases, depending on the values of NumEvents and Amount
	if (Quests[0]->CurrentObjective->NumEvents == 1 && Quests[0]->CurrentObjective->Amount == 1) CompleteObjective();
	else if (Quests[0]->CurrentObjective->NumEvents == 1 && Quests[0]->CurrentObjective->Amount > 1)
	{
		//UE_LOG(LogTemp, Warning, TEXT("In UQuestComponent::OnQuestEventCreated(): In Case 2."));
		if (ObjectiveType != EObjectiveType::EOT_AttributeUpdated)
		{
			Quests[0]->CurrentObjective->Progress += Amount;
			//UE_LOG(LogTemp, Warning, TEXT("In UQuestComponent::OnQuestEventCreated(): Not Attribute. Progress: %i, QAmount: %i"), Quests[0]->CurrentObjective->Progress, Quests[0]->CurrentObjective->Amount);

			if (Quests[0]->CurrentObjective->Progress >= Quests[0]->CurrentObjective->Amount) CompleteObjective();
		}
		else if (Amount >= Quests[0]->CurrentObjective->Amount) CompleteObjective();
	}
	else if (Quests[0]->CurrentObjective->NumEvents > 1 && Amount == 1)
	{
		Quests[0]->CurrentObjective->Progress += 1;
		//UE_LOG(LogTemp, Warning, TEXT("In UQuestComponent::OnQuestEventCreated(): In Case 3. Progress: %i, NumEvents: %i"), Quests[0]->CurrentObjective->Progress, Quests[0]->CurrentObjective->NumEvents);
		if (Quests[0]->CurrentObjective->Progress >= Quests[0]->CurrentObjective->NumEvents) CompleteObjective();
	}
	else if (Quests[0]->CurrentObjective->NumEvents > 1 && Quests[0]->CurrentObjective->Amount > 1)
	{
		//UE_LOG(LogTemp, Warning, TEXT("In UQuestComponent::OnQuestEventCreated(): In Case 4."));
		if (Amount > Quests[0]->CurrentObjective->Amount) Quests[0]->CurrentObjective->Progress += 1;

		//UE_LOG(LogTemp, Warning, TEXT("In UQuestComponent::OnQuestEventCreated(): Progress: %i, NumEvents: %i"), Quests[0]->CurrentObjective->Progress, Quests[0]->CurrentObjective->NumEvents);
		if (Quests[0]->CurrentObjective->Progress > Quests[0]->CurrentObjective->NumEvents) CompleteObjective();
	}
}

void UQuestComponent::Setup(UQuestsWidget* InputWidget)
{
	QuestWidget = InputWidget;

	if (QuestWidget)
	{
		QuestWidget->SetQuestText(Quests[0]->DisplayName);
		QuestWidget->SetObjectiveText(Quests[0]->CurrentObjective->DisplayName);
	}
}

void UQuestComponent::CompleteObjective()
{
	Quests[0]->CurrentObjective->bComplete = true;
	Quests[0]->CurrentObjective = Quests[0]->CurrentObjective->NextObjective;

	if (Quests[0]->CurrentObjective == nullptr)
	{
		CompleteQuest();
	}
	else
	{
		QuestWidget->SetObjectiveText(Quests[0]->CurrentObjective->DisplayName);
	}
}

void UQuestComponent::CompleteQuest()
{
	Quests[0]->bComplete = true;
	UEventsSubsystem* EventsSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UEventsSubsystem>();
	EventsSubsystem->OnEventCreatedDelegate.RemoveDynamic(this, &UQuestComponent::OnQuestEventCreated);
	QuestWidget->SetQuestText("");
	QuestWidget->SetObjectiveText("");
	QuestWidget->Hide();
}
