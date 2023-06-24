// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

class AItem;
class UInventory;

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
	void SetLevelInfo(int32 Level, float LevelPercent);
	void OpenEverythingMenu(int32 Souls, int32 SoulsNeeded, int32 NextLevel, UInventory* OtherInventory = nullptr);
	void UpdateAttribute(int32 ItemID, float Value);

	UFUNCTION(BlueprintImplementableEvent, Category = "Overlay")
	void ShowTabWidget(int32 Souls, int32 SoulsNeeded, int32 NextLevel, UInventory* OtherInventory = nullptr);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Overlay")
	void UpdateTabWidget(int32 Souls, int32 SoulsNeeded, int32 NextLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Overlay")
	void ShowMerchantMenu(const TArray<int32>& ItemsToSell, const TArray<int32>& ItemsToBuy, int32 UserGold);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Merchant")
	void UpdateGold(int32 UserGold);

	UFUNCTION(BlueprintImplementableEvent, Category = "Overlay")
	void ShowDeathWidget();

	/*
	* WBPs implement SetItemPickupText() and HideItemPickupText()
	* When UpdateItemPickupText() is called (from anywhere that calls Inventory->AddItem(), basically),
	*  it sets a timer for how long until it's time to clear the Item Text
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Overlay")
	void SetItemPickupText(int32 ItemID, int32 Amount);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Overlay")
	void HideItemPickupText();
	
	void UpdateItemPickupText(int32 ItemID, int32 Amount);
	void ClearItemPickupText();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Overlay")
	bool CtrlPressed();
	
	/* A series of functions to help the user Inventory and the container Inventory talk to each other */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Overlay")
	bool ContainerIsOpen();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Overlay")
	UInventory* GetContainerInventory();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Overlay")
	void RefreshContainer();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Overlay")
	UInventory* GetUserInventory();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Overlay")
	void RefreshUserInventory();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateInteractMsg(const FString& ActorName, const FString& InteractWord);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideInteractMsg();


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
	UTextBlock* LevelText;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* LevelProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	class UQuestsWidget* QuestWidget;

	FTimerHandle ClearItemPickupTimer;

	float TimeUntilItemTextCleared = 3.f;

public:
	FORCEINLINE UQuestsWidget* GetQuestWidget() const { return QuestWidget; }
};
