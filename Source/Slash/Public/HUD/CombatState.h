// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatState.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class SLASH_API UCombatState : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetStateAlerting();
	void SetStateCombat();

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* IndicatorText;
};
