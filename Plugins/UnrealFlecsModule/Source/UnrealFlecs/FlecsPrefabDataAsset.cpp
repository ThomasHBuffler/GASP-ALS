// Fill out your copyright notice in the Description page of Project Settings.


#include "FlecsPrefabDataAsset.h"


void UFlecsPrefabDataAsset::RegisterPrefab(flecs::world* World)
{
	Prefab = World->prefab();
	
	for (auto Component : Components)
	{
		if (IsValid(Component)) Component->AddComponentToEntity(Prefab);
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, "Prefab Registered");
}