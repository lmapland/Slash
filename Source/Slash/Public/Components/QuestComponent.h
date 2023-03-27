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
	int32 Amount = -1;
	
	FObjective* NextObjective = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bComplete = false;

};

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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnQuestEventCreated(EObjectiveType ObjectiveType, int32 ID, int32 Amount);

protected:
	virtual void BeginPlay() override;

private:
	TArray<FQuest*> Quests;
		
};
