// Copyright Epic Games, Inc. All Rights Reserved.

#include "AdditionalBlueprintNodesModule.h"
#include "AssetBlueprintGraphActions.h"
#include "EdGraphSchema_K2_Actions.h"
#include "TickableEditorObject.h"
#include "UObject/UObjectIterator.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "AdditionalBlueprintNodes"

struct FEnhancedSettingGraphActions : public FAssetBlueprintGraphActions
{
	virtual FText GetGraphHoverMessage(const FAssetData& AssetData, const UEdGraph* HoverGraph) const override;
	virtual bool TryCreatingAssetNode(const FAssetData& AssetData, UEdGraph* ParentGraph, const FVector2D Location, EK2NewNodeFlags Options) const override;
};

FText FEnhancedSettingGraphActions::GetGraphHoverMessage(const FAssetData& AssetData, const UEdGraph* HoverGraph) const
{
	return FText::FromName(AssetData.AssetName); //FText::Format(LOCTEXT("EnhancedSettingHoverMessage", "{0}"), FText::FromName(AssetData.AssetName));
}

bool FEnhancedSettingGraphActions::TryCreatingAssetNode(const FAssetData& AssetData, UEdGraph* ParentGraph, const FVector2D Location, EK2NewNodeFlags Options) const
{
	/*
	if (AssetData.IsValid())
	{
		if (const UEnhancedSetting* Action = Cast<const UEnhancedSetting>(AssetData.GetAsset()))
		{
			for (const TObjectPtr<UEdGraphNode>& Node : ParentGraph->Nodes)
			{
				if(const UK2Node_EnhancedSetting* EnhancedSettingNode = Cast<UK2Node_EnhancedSetting>(Node))
				{
					if (EnhancedSettingNode->EnhancedSetting.GetFName() == AssetData.AssetName)
					{
						if (const TSharedPtr<IBlueprintEditor> BlueprintEditor = FKismetEditorUtilities::GetIBlueprintEditorForObject(ParentGraph, false))
						{
							BlueprintEditor.Get()->JumpToPin(EnhancedSettingNode->GetPinAt(0));
						}
						
						return false;
					}
				}
			}

			UK2Node_EnhancedEnhancedSetting* NewNode = FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_EnhancedEnhancedSetting>(
				ParentGraph,
				Location,
				Options,
				[Action](UK2Node_EnhancedEnhancedSetting* NewInstance)
				{
					NewInstance->EnhancedSetting = Action;
				}

			);
			return true;
		}
	}*/
	return false;
}



class FEnhancedSettingsBlueprintNodesModule : public IModuleInterface, public FTickableEditorObject
{
	// IModuleInterface interface
	virtual void StartupModule() override
	{
		// Register graph actions:
		FBlueprintGraphModule& GraphModule = FModuleManager::LoadModuleChecked<FBlueprintGraphModule>("BlueprintGraph");
		{
			//GraphModule.RegisterGraphAction(UEnhancedSetting::StaticClass(), MakeUnique<FEnhancedSettingGraphActions>());
		}
	}
	// End IModuleInterface interface

	// FTickableEditorObject interface
	virtual void Tick(float DeltaTime) override
	{
	/*
		// Update any blueprints that are referencing an input action with a modified value type
		if (UEnhancedSetting::ActionsWithModifiedValueTypes.Num() || UEnhancedSetting::ActionsWithModifiedTriggers.Num())
		{
			TSet<UBlueprint*> BPsModifiedFromValueTypeChange;
			TSet<UBlueprint*> BPsModifiedFromTriggerChange;
			
			for (TObjectIterator<UK2Node_EnhancedEnhancedSetting> NodeIt; NodeIt; ++NodeIt)
			{
				if (!FBlueprintNodeTemplateCache::IsTemplateOuter(NodeIt->GetGraph()))
				{
					if (UEnhancedSetting::ActionsWithModifiedValueTypes.Contains(NodeIt->EnhancedSetting))
					{
						NodeIt->ReconstructNode();
						BPsModifiedFromValueTypeChange.Emplace(NodeIt->GetBlueprint());
					}
					if (UEnhancedSetting::ActionsWithModifiedTriggers.Contains(NodeIt->EnhancedSetting))
					{
						NodeIt->ReconstructNode();
						BPsModifiedFromTriggerChange.Emplace(NodeIt->GetBlueprint());
					}
				}
			}
			for (TObjectIterator<UK2Node_GetEnhancedSettingValue> NodeIt; NodeIt; ++NodeIt)
			{
				if (!FBlueprintNodeTemplateCache::IsTemplateOuter(NodeIt->GetGraph()))
				{
					if (UEnhancedSetting::ActionsWithModifiedValueTypes.Contains(NodeIt->EnhancedSetting))
					{
						NodeIt->ReconstructNode();
						BPsModifiedFromValueTypeChange.Emplace(NodeIt->GetBlueprint());
					}
					if (UEnhancedSetting::ActionsWithModifiedTriggers.Contains(NodeIt->EnhancedSetting))
					{
						NodeIt->ReconstructNode();
						BPsModifiedFromTriggerChange.Emplace(NodeIt->GetBlueprint());
					}
				}
			}

			if (BPsModifiedFromValueTypeChange.Num())
			{
				FNotificationInfo Info(FText::Format(LOCTEXT("ActionValueTypeChange", "Changing action value type affected {0} blueprint(s)!"), BPsModifiedFromValueTypeChange.Num()));
				Info.ExpireDuration = 5.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
			}
			if (BPsModifiedFromTriggerChange.Num())
			{
				FNotificationInfo Info(FText::Format(LOCTEXT("ActionTriggerChange", "Changing action triggers affected {0} blueprint(s)!"), BPsModifiedFromTriggerChange.Num()));
				Info.ExpireDuration = 5.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
			}

			UEnhancedSetting::ActionsWithModifiedValueTypes.Reset();
			UEnhancedSetting::ActionsWithModifiedTriggers.Reset();
			

		}*/
	}
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FInputBlueprintNodesModule, STATGROUP_Tickables); }
	// End FTickableEditorObject interface
};

IMPLEMENT_MODULE(FEnhancedSettingsBlueprintNodesModule, EnhancedSettingsBlueprintNodes)

#undef LOCTEXT_NAMESPACE
