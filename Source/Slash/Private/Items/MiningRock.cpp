// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/MiningRock.h"

AMiningRock::AMiningRock()
{
	ItemName = FName("Ore");
	AnimationToPlay = FName("Mining");
	AnimSpeed = 1.f;
}