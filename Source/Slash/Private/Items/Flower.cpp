// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Flower.h"

AFlower::AFlower()
{
	ItemName = FName("Flower");
	AnimationToPlay = FName("PickUp");
	AnimSpeed = 2.f;
}