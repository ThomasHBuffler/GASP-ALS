// Fill out your copyright notice in the Description page of Project Settings.


#include "ZxyrisPlayerCameraManager.h"

AZxyrisPlayerCameraManager::AZxyrisPlayerCameraManager()
{
	LastFOV = DefaultFOV; 
}

void AZxyrisPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);

	// Check if the FOV has changed
	if (LastFOV != GetFOVAngle())
	{
		// Broadcast the FOV change
		OnFOVChanged.Broadcast(GetFOVAngle());

		// Update the LastFOV to the current FOV
		LastFOV = GetFOVAngle();
	}
}