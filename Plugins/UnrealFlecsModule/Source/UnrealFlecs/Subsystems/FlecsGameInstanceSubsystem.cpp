// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsGameInstanceSubsystem.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealFlecs/Flecs/flecs.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealFlecs/Systems/FlecsSystems.h"

class UFlecsPrefabDataAsset;

void UFlecsGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	OnTickDelegate = FTickerDelegate::CreateUObject(this, &UFlecsGameInstanceSubsystem::Tick);
	OnTickHandle = FTSTicker::GetCoreTicker().AddTicker(OnTickDelegate);
	
	//go to https://www.flecs.dev/explorer/ when the project is running to inspect active entities and values
	char* argv[] = {"Unreal Flecs - Game Instance Subsystem"};
	FlecsWorld = new flecs::world(1, argv);
	
	LoadPrefabDataAssets();
	UFlecsSystems::InitializeSystems(GetWorld(), FlecsWorld, this);
	
	UE_LOG(LogTemp, Warning, TEXT("UFlecsGameInstanceSubsystem has been Initialized!"));
	Super::Initialize(Collection);
}

void UFlecsGameInstanceSubsystem::LoadPrefabDataAssets()
{
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	if (AssetRegistry.IsLoadingAssets())
	{
		AssetRegistry.OnFilesLoaded().AddUObject(this, &UFlecsGameInstanceSubsystem::OnAssetRegistryLoaded);
		const FString IsLoading = "Asset registry is still loading assets, Queuing";
		UE_LOG(LogTemp, Log, TEXT("%s"), *IsLoading);
	}
	else
	{
		LoadPrefabAssets();
	}

	/*
	GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Green, "Loading Prefab Data Assets");
	TArray<FPrimaryAssetId> OutPrimaryAssetIdList;
	UKismetSystemLibrary::GetPrimaryAssetIdList(TEXT("FlecsPrefab"), OutPrimaryAssetIdList);

	for (auto& PrimaryAssetId : OutPrimaryAssetIdList)
	{
		FPrimaryAssetType PrimaryAssetType = PrimaryAssetId.PrimaryAssetType;
		FName PrimaryAssetName = PrimaryAssetId.PrimaryAssetName;
		UFlecsPrefabDataAsset* FlecsPrefabDataAsset = Cast<UFlecsPrefabDataAsset>(UAssetManager::Get().GetPrimaryAssetObject(PrimaryAssetId));
		if (FlecsPrefabDataAsset)
		{
			FlecsPrefabDataAsset->RegisterPrefab(GetFlecsWorld());
		}
	}*/
}

void UFlecsGameInstanceSubsystem::OnAssetRegistryLoaded()
{
	LoadPrefabAssets();
}

void UFlecsGameInstanceSubsystem::OnPrefabAssetsLoadedAsync()
{
	GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, "On Settings Assets Loaded");

	UE_LOG(LogTemp, Log, TEXT("OnSettingsAssetsLoaded called."));

	// Populate SettingProfile.Values after assets are loaded
	const UAssetManager& AssetManager = UAssetManager::Get();
	const FPrimaryAssetType SettingsAssetType = TEXT("FlecsPrefabDataAsset");

	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(SettingsAssetType, AssetIds);

	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		AddPrefab(Cast<UFlecsPrefabDataAsset>(AssetManager.GetPrimaryAssetPath(AssetId).TryLoad()));
	}

	PostGatherPrefabs();
}

void UFlecsGameInstanceSubsystem::PostGatherPrefabs()
{
	GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, "Post Gather Prefabs");
}

void UFlecsGameInstanceSubsystem::LoadPrefabAssets()
{
	UE_LOG(LogTemp, Log, TEXT("LoadPrefabAssets called."));
	if (!UAssetManager::IsInitialized())
	{
		UE_LOG(LogTemp, Warning, TEXT("AssetManager is not initialized."));
		return;
	}
	const UAssetManager& AssetManager = UAssetManager::Get();
	const FPrimaryAssetType SettingsAssetType = FPrimaryAssetType("FlecsPrefabDataAsset");

	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(SettingsAssetType, AssetIds);

	// Convert PrimaryAssetId to SoftObjectPath
	TArray<FSoftObjectPath> AssetPaths;
	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(AssetId);
		AssetPaths.Add(AssetPath);
	}

	// Log the number of assets found
	UE_LOG(LogTemp, Log, TEXT("Found %d Setting assets in %s"), AssetPaths.Num(), *GetOuter()->GetClass()->GetName());

	// Log each AssetPath for verification
	for (const FSoftObjectPath& Path : AssetPaths)
	{
		UE_LOG(LogTemp, Log, TEXT("Loading Setting Asset: %s"), *Path.ToString());
	}

	if (bUseAsyncLoading)
	{
		// Request asynchronous load
		FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
		StreamableManager.RequestAsyncLoad(
			AssetPaths, FStreamableDelegate::CreateUObject(
				this, &UFlecsGameInstanceSubsystem::OnPrefabAssetsLoadedAsync));
	}
	else
	{
		// Load the assets synchronously
		for (const FSoftObjectPath& Path : AssetPaths)
		{
			AddPrefab(Cast<UFlecsPrefabDataAsset>(Path.TryLoad()));
		}

		PostGatherPrefabs();
	}
}

void UFlecsGameInstanceSubsystem::AddPrefab(UFlecsPrefabDataAsset* Prefab)
{
	if (Prefab)
	{
		Prefabs.Add(Prefab);
		Prefab->RegisterPrefab(GetFlecsWorld());
	}
}

void UFlecsGameInstanceSubsystem::SpawnProjectileEntityFromPrefab(AActor* Instigator, UFlecsPrefabDataAsset* FlecsPrefabDataAsset, FVector Location, FVector MuzzleLocation, FVector Forward)
{
	if (!IsValid(FlecsPrefabDataAsset))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "FlecsPrefabDataAsset is not valid");
		return;
	}
	
	if (!Prefabs.Contains(FlecsPrefabDataAsset))
	{
		AddPrefab(FlecsPrefabDataAsset);
	}
	
	auto Projectile = FlecsWorld->entity().is_a(FlecsPrefabDataAsset->GetPrefab(GetFlecsWorld()));
	
	Projectile.set<FFlecsInstigator>({Instigator});
	Projectile.set<FFlecsDeltaLocation>({});
	
	if (Projectile.has<FFlecsProjectileSpeed>())
	{
		Projectile.set<FFlecsVelocity>({Forward * Projectile.get<FFlecsProjectileSpeed>()->Value});
	}
	
	Projectile.set<FFlecsLocation>({Location});
	Projectile.set<FFlecsVisualOffset>({ MuzzleLocation - Location });
	
	if (Projectile.has<FFlecsProjectileNiagaraSystem>())
	{
		if (UNiagaraSystem* NiagaraSystem = Projectile.get<FFlecsProjectileNiagaraSystem>()->Value)
		{
			UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, MuzzleLocation, Forward.Rotation(), FVector::OneVector, true, true, ENCPoolMethod::ManualRelease, false);

			Projectile.set<FFlecsProjectileNiagaraComponent>({NiagaraComponent});
		
			NiagaraComponent->SetBoolParameter("Alive", true);

			NiagaraComponentTestTrackers.AddUnique(NiagaraComponent);
		}
	}
}


void UFlecsGameInstanceSubsystem::SpawnEntityFromPrefab(UFlecsPrefabDataAsset* FlecsPrefabDataAsset)
{
	FlecsWorld->entity().is_a(FlecsPrefabDataAsset->GetPrefab(GetFlecsWorld()));
}

bool UFlecsGameInstanceSubsystem::Tick(float DeltaTime)
{
	FlecsWorld->progress(DeltaTime * UGameplayStatics::GetGlobalTimeDilation(GetWorld()));
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, "FlecsWorld Progressed with a DeltaTime of " + FString::SanitizeFloat(DeltaTime));
	return true;
	
	// Print if the NiagaraComponentTestTracker is valid
	int Valid = 0;
	int Invalid = 0;
	int loops = 0;
	
	for (auto& NiagaraComponentTestTracker : NiagaraComponentTestTrackers)
	{
		if (NiagaraComponentTestTracker.IsValid())
		{
			Valid++;
			
		}
		else
		{
			Invalid++;
		}
		loops++;
	}
	
	FString DebugMessage = "Valid: " + FString::FromInt(Valid) + " Invalid: " + FString::FromInt(Invalid);
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, DebugMessage);
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, "Loops: " + FString::FromInt(loops));
	return true;
}

void UFlecsGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}
