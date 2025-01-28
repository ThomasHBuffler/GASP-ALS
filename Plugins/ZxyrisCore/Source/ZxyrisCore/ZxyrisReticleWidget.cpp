// Fill out your copyright notice in the Description page of Project Settings.


#include "ZxyrisReticleWidget.h"

#include "ZxyrisPlayerCameraManager.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"

class UZxyrisCameraComponent;

void UReticleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!FViewport::ViewportResizedEvent.IsBoundToObject(this))
	{
		FViewport::ViewportResizedEvent.AddUObject(this, &UReticleWidget::OnViewportResized);
	}
	
	// Get the camera manager and set the initial FOV, bind to the FOV changed event in our custom camera manager
	OnFOVChanged(90);
	if (GetOwningPlayer())
	{
		if (AZxyrisPlayerCameraManager* CameraManager = Cast<AZxyrisPlayerCameraManager>(GetOwningPlayer()->PlayerCameraManager))
		{
			OnFOVChanged(CameraManager->GetFOVAngle());
			CameraManager->OnFOVChanged.AddDynamic(this, &UReticleWidget::OnFOVChanged);
		}
	}

	// Manually call the initial broadcast, 1 frame after construction
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UReticleWidget::InitialBroadcast, 0.0001f, false);
}

void UReticleWidget::InitialBroadcast()
{
	OnSpreadInPixelsChanged(SpreadInPixels);
}

void UReticleWidget::RecalculateCachedValues()
{
	Upscale = 1 / UWidgetLayoutLibrary::GetViewportScale(this);
	SpreadToFOV = 1 / UKismetMathLibrary::DegTan(VerticalFOV / 2);
}

void UReticleWidget::OnViewportResized(FViewport* Viewport, uint32 Index)
{
	RecalculateCachedValues();
	SetReticleSpread(SpreadInDegrees);
}

void UReticleWidget::OnFOVChanged(float NewHorizontalFOV)
{
	HorizontalFOV = NewHorizontalFOV;
	VerticalFOV = UKismetMathLibrary::DegAtan(UKismetMathLibrary::DegTan(HorizontalFOV / 2) / DefaultAspectRatio) * 2;
	RecalculateCachedValues();
	SetReticleSpread(SpreadInDegrees);
}

void UReticleWidget::SetReticleSpread(const double NewSpreadInDegrees)
{
	SpreadInDegrees = NewSpreadInDegrees;
	const int ScreenHeight = UWidgetLayoutLibrary::GetViewportSize(this).Y;
	const int NewSpreadInPixels = FMath::CeilToInt(ScreenHeight * (UKismetMathLibrary::DegTan(SpreadInDegrees / 2) * SpreadToFOV) * Upscale);
	if (NewSpreadInPixels == SpreadInPixels) return;
	SpreadInPixels = NewSpreadInPixels;
	OnSpreadInPixelsChanged(SpreadInPixels);
}
