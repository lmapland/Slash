// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

class USlashOverlay;

/**
 * The parent class of the HUD object
 * This class adds the SlashOverlay class defined here to the HUD
 * (probably WBP_Overlay)
 */
UCLASS()
class SLASH_API ASlashHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Slash)
	TSubclassOf<USlashOverlay> SlashOverlayClass;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE USlashOverlay* GetSlashOverlay() const { return SlashOverlay; }
};
