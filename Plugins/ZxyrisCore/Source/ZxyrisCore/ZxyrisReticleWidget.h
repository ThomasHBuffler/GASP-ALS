// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZxyrisReticleWidget.generated.h"

/**
 * Widget that displays a reticle with spread
 * (Override the OnSpreadInPixelsChanged event to update the reticle)
 *
 * Suggested layout
 * CanvasPanel->CanvasPanel->Crosshair images on each side outside the canvas panel.
 * AsCanvasSlot->SetSize(FVector2D(SpreadInPixels, SpreadInPixels))
 */
UCLASS()
class ZXYRISCORE_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reticle")
	double DefaultAspectRatio = 16.0 / 9.0;

	double SpreadInDegrees = 0.0;
	int SpreadInPixels;
	
	double HorizontalFOV;
	double VerticalFOV;
	
	double Upscale;
	
	double SpreadToFOV;
		
public:
	void OnViewportResized(FViewport* Viewport, uint32 Index);

	UFUNCTION()
	void InitialBroadcast();
	
	virtual void NativeConstruct() override;
	void RecalculateCachedValues();

	UFUNCTION(BlueprintCallable, Category = "Reticle")
	void SetReticleSpread(double NewSpreadInDegrees);

	UFUNCTION()
	void OnFOVChanged(float NewHorizontalFOV);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reticle")
	void OnSpreadInPixelsChanged(int NewSpreadInPixels);
};
