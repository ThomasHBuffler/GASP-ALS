// Copyright (c) 2024 Thomas Hawke Buffler. All rights reserved.This software and its accompanying documentation are the proprietary property of [Your Full Name]. Unauthorized copying, distribution, modification, or use of this software, in whole or in part, is strictly prohibited without prior written permission from the copyright owner.For inquiries, contact: ThomasHBufflerAccess@gmail.com

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReticleWidget.generated.h"

// On spread in pixels changed delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpreadInPixelsChanged, int32, SpreadInPixels);

UCLASS()
class SHOOTERCORE_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()

	void OnViewportResized(FViewport* Viewport, uint32 ID);
	virtual void NativeConstruct() override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Reticle")
	void InitialUpdate(float InSpreadInDegrees);
	
	UFUNCTION(BlueprintCallable, Category = "Reticle")
	void RecalculateBaseVariables();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Reticle")
	void OnSpreadInPixelsChanged(int InSpreadInPixels);

	// Cached variables
	float Upscale;
	double SpreadToFOV;
	float SpreadInDegrees;
	int SpreadInPixels;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Reticle")
	FOnSpreadInPixelsChanged OnSpreadInPixelsChangedDelegate;


	UFUNCTION(BlueprintCallable, Category = "Reticle")
	void SetSpread(float InSpreadInDegrees);
};
