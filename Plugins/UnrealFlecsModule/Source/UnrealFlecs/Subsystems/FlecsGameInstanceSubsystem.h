// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealFlecs/Flecs/flecs.h"
#include "UnrealFlecs/FlecsPrefabDataAsset.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FlecsGameInstanceSubsystem.generated.h"

class UNiagaraComponent;

/**
 *
 */
UCLASS()
class UNREALFLECS_API UFlecsGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UFlecsPrefabDataAsset*> Prefabs; 
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void LoadPrefabDataAssets();
	void OnAssetRegistryLoaded();
	void  OnPrefabAssetsLoadedAsync();
	void PostGatherPrefabs();
	void LoadPrefabAssets();
	void AddPrefab(UFlecsPrefabDataAsset* Prefab);

	//void OnAssetLoaded();
	virtual void Deinitialize() override;
	bool Tick(float DeltaTime);
	
protected:
	FTickerDelegate OnTickDelegate;
	FTSTicker::FDelegateHandle OnTickHandle;
	
	bool bUseAsyncLoading = true;
	
	static inline flecs::world* FlecsWorld = nullptr;

public:
	TArray<TWeakObjectPtr<UNiagaraComponent>> NiagaraComponentTestTrackers;
	
	static flecs::world* GetFlecsWorld(){ return FlecsWorld; }
	
	UFUNCTION(BlueprintCallable)
	void SpawnEntityFromPrefab(UFlecsPrefabDataAsset* FlecsPrefabDataAsset);
	
	UFUNCTION(BlueprintCallable)
	void SpawnProjectileEntityFromPrefab(AActor* Instigator, UFlecsPrefabDataAsset* FlecsPrefabDataAsset, FVector Location, FVector MuzzleLocation, FVector Forward);
};