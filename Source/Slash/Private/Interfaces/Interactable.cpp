// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/Interactable.h"

// Add default functionality here for any IInteractable functions that are not pure virtual.
void IInteractable::Interact(USlashOverlay* Overlay, UAttributeComponent* Attributes)
{

}

FString IInteractable::GetActorName()
{
    return FString();
}

FString IInteractable::GetInteractWord()
{
    return FString();
}
