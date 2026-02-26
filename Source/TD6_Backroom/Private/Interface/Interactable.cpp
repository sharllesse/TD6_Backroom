// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/Interactable.h"


void IInteractable::OnBeginFocus()
{
}

void IInteractable::OnEndFocus()
{
}

// Add default functionality here for any IInteractable functions that are not pure virtual.
void IInteractable::OnInteract(AActor* Instigator)
{
}

FText IInteractable::GetInteractText() const
{
	return FText::FromString(TEXT("Interact"));
}
