// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

bool UHealthComponent::ApplyDamage(float Damage, float& AppliedDamage)
{
	std::lock_guard<std::mutex> Lock(Mutex);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Health: %f"), Health));
	bool bWasDamageApplied = false;
	if (Health >= 0)
	{
		AppliedDamage = FMath::Clamp(Damage, 0.f, Health);
		Health -= AppliedDamage;
			
		bWasDamageApplied = true;
	}
		
	return bWasDamageApplied;
}
