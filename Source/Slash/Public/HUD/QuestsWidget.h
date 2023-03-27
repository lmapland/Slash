// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestsWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class SLASH_API UQuestsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetQuestText(FString QuestName);
	void SetObjectiveText(FString ObjectiveName);
	void Hide();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuestText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ObjectiveText;

};
