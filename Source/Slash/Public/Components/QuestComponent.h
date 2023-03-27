// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/QuestObjectiveTypes.h"
#include "QuestComponent.generated.h"

USTRUCT(Blueprintable)
struct FQuest
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bComplete = false;

	FObjective* CurrentObjective = nullptr;

	TArray<FObjective*> Objectives;
};

/*
* Notes on ObjectiveType:
* EOT_ItemAdded will send in how many items were added
* EOT_AttributeUpdated will send in what the Attribute was updated TO
*  This is an important distinction! It won't say how much the Attribute was updated by, only what the new value is!
*/
USTRUCT(Blueprintable)
struct FObjective
{
	GENERATED_USTRUCT_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EObjectiveType Type;

	/* If the ID is not set, then the objective is non-specific (doesn't care WHICH item was sold / purchased / equipped) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ID = -1;

	/* Number of times to catch the event firing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumEvents = 1;
	
	/* An Amount passed into the event
	* Will do [EventAmount] >= [ObjectiveAmount] logic
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Progress = 0;
	
	FObjective* NextObjective = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bComplete = false;
};

class UQuestsWidget;

/*
* This is so barebones
* In the future, probably these will be stored in a database
* Right now there is only the Quests array, but there needs to be a place to store old quests
*  vs current quests. Old quests shouldn't be consulted / take up space or time when events arrive.
* But since I have only 1 quest at the moment I'm not worried about it. If I choose to expand quests
*  I will overhaul the system.
* Also, I create the intro quest in the initialization function because there is no save & load feature.
* The quest is one that should be auto-bestowed on new players, but checks need to be in place that it
*  is not auto-bestowed when a player is loading a saved game.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestComponent();

	UFUNCTION()
	void OnQuestEventCreated(EObjectiveType ObjectiveType, int32 ID, int32 Amount);

	UFUNCTION()
	void Setup(UQuestsWidget* InputWidget);

protected:
	virtual void BeginPlay() override;

private:
	void CompleteObjective();

	void CompleteQuest();

	TArray<FQuest*> Quests;

	UQuestsWidget* QuestWidget;

};
