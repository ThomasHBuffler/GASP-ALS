// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mutex>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UCLASS(Blueprintable, Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALFLECS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent() {}
	
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*	Applies damage to the health component (Thread safe)
	 *  
	 *  @param Damage The amount of damage to apply.
	 *  @param Applied The amount of damage that was applied.
	 *
	 *  @return True if the damage was applied (Return Param).
	*/
	virtual bool ApplyDamage(float Damage, float& AppliedDamage);
	
protected:
	//virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float Health = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.f;
	
	std::mutex Mutex;	

};
