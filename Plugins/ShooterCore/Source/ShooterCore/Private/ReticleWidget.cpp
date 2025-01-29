#include "ReticleWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealClient.h"

void UReticleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RecalculateBaseVariables();	
	if (!OnSpreadInPixelsChangedDelegate.IsBound()) OnSpreadInPixelsChangedDelegate.AddDynamic(this, &ThisClass::OnSpreadInPixelsChanged);
	if (!FViewport::ViewportResizedEvent.IsBoundToObject(this)) FViewport::ViewportResizedEvent.AddUObject(this, &ThisClass::OnViewportResized);
}

void UReticleWidget::RecalculateBaseVariables()
{
	if (!IsValid(GetOwningPlayerCameraManager())) return;
	Upscale = 1 / UWidgetLayoutLibrary::GetViewportScale(this);
	SpreadToFOV = 1 / UKismetMathLibrary::DegTan(GetOwningPlayerCameraManager()->GetFOVAngle() / 2);
	SetSpread(SpreadInDegrees);
}

void UReticleWidget::OnViewportResized(FViewport* Viewport, uint32 ID)
{
	RecalculateBaseVariables();
}

void UReticleWidget::SetSpread(float InSpreadInDegrees)
{
	SpreadInDegrees = InSpreadInDegrees;
	int ScreenWidth = GetCachedGeometry().GetAbsoluteSize().X;
	OnSpreadInPixelsChangedDelegate.Broadcast(SpreadInPixels = FMath::RoundToInt(ScreenWidth * (UKismetMathLibrary::DegTan(InSpreadInDegrees) * SpreadToFOV / 2) * Upscale));
}

void UReticleWidget::InitialUpdate(float InSpreadInDegrees)
{
	SpreadInDegrees = InSpreadInDegrees;
	RecalculateBaseVariables();
	SetSpread(SpreadInDegrees);
}
