// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ZxyrisPlayerCameraManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFieldOfViewChanged, float, FieldOfView);

UCLASS()
class ZXYRISCORE_API AZxyrisPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AZxyrisPlayerCameraManager();
	
	UPROPERTY(BlueprintAssignable, Category = "Camera")
	FOnFieldOfViewChanged OnFOVChanged;

protected:
	virtual void UpdateCamera(float DeltaTime) override;

private:
	float LastFOV;
};
