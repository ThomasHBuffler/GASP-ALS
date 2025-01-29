// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/FlecsExampleComponents.h"
#include "Engine/DataAsset.h"
#include "Flecs/flecs.h"
#include "FlecsPrefabDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class UNREALFLECS_API UFlecsPrefabDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	flecs::entity Prefab;
		
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName DevName;

	void RegisterPrefab(flecs::world* World);
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("FlecsPrefab", GetFName());
	}
	
	flecs::entity GetPrefab(flecs::world* World)
	{
		/*
		if (!bIsInitialized)
		{
			RegisterPrefab(World);
			bIsInitialized = true;
		}*/
		return Prefab;
	}
	
	UPROPERTY(Transient)
	bool bIsInitialized = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<UFlecsEditorFragment*> Components;

	/*
	 *	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<UTimedEvent*> TimedEvents;*/
};