// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

class AItem;

/**
 * 
 */
UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);
	void SetFlowers(int32 Flowers);
	void SetOre(int32 Ore);
	void SetLevelInfo(int32 Level, float LevelPercent);
	void OpenEverythingMenu(int32 Souls, int32 SoulsNeeded, int32 NextLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Overlay")
	void ShowTabWidget(int32 Souls, int32 SoulsNeeded, int32 NextLevel);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Overlay")
	void UpdateTabWidget(int32 Souls, int32 SoulsNeeded, int32 NextLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Overlay")
	void ShowMerchantMenu(const TArray<TSubclassOf<AItem>>& ItemsToSell, const TArray<TSubclassOf<AItem>>& ItemsToBuy);
	
private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulsText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FlowersText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* OreText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelText;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* LevelProgressBar;
};
