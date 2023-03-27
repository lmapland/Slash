// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/QuestsWidget.h"
#include "Components/TextBlock.h"

void UQuestsWidget::SetQuestText(FString QuestName)
{
	if (QuestText)
	{
		QuestText->SetText(FText::FromString(QuestName));
	}
}

void UQuestsWidget::SetObjectiveText(FString ObjectiveName)
{
	if (ObjectiveText)
	{
		ObjectiveText->SetText(FText::FromString(ObjectiveName));
	}
}

void UQuestsWidget::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}
