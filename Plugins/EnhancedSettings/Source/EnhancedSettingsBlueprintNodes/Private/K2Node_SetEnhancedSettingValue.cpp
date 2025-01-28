// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_SetEnhancedSettingValue.h"

#include "EnhancedSetting.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "Editor.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_EnhancedSettingValueModifier.h"
#include "K2Node_Self.h"
#include "Misc/PackageName.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Modules/ModuleManager.h"
#include "EnhancedSettingsTypes.h"
#include "NodeMetadata.h"

#define LOCTEXT_NAMESPACE "K2Node_SetEnhancedSettingValue"

static const FName ValuePinName = TEXT("Value");

void UK2Node_SetEnhancedSettingValue::AllocateDefaultPins()
{
	PreloadObject(static_cast<UObject*>(EnhancedSetting));

	Super::AllocateDefaultPins();
	
	if (!IsValid(EnhancedSetting)) return;
	
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	
	if (EnhancedSetting->GetEnhancedSettingScope() == LocalPlayer)
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, APlayerController::StaticClass(), TEXT("PlayerController"));
	}
	
	// Dynamically typed input
	FName SubCategory = GetValueSubCategory(EnhancedSetting);

	if (SubCategory != NAME_None)
	{
		CreatePin(EGPD_Input, GetValueCategory(EnhancedSetting), SubCategory, ValuePinName);
	}
	else
	{
		CreatePin(EGPD_Input, GetValueCategory(EnhancedSetting), GetValueSubCategoryObject(EnhancedSetting), ValuePinName);
	}

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
}

void UK2Node_SetEnhancedSettingValue::Initialize(const UEnhancedSetting* InSetting)
{
	EnhancedSetting = InSetting;
}

void UK2Node_SetEnhancedSettingValue::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	
	if (!EnhancedSetting) return;
	
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	
	// Accessor does the call to the function
	UK2Node_EnhancedSettingValueModifier* ModifierNode = CompilerContext.SpawnIntermediateNode<UK2Node_EnhancedSettingValueModifier>(this, SourceGraph);
	ModifierNode->Initialize(EnhancedSetting);
	ModifierNode->AllocateDefaultPins();
	
	if (EnhancedSetting->GetEnhancedSettingScope() == LocalPlayer)
	{
		UEdGraphPin* PCPinIn = FindPinChecked(TEXT("PlayerController"));
		
		CompilerContext.MovePinLinksToIntermediate(*PCPinIn, *ModifierNode->FindPinChecked(TEXT("PlayerController")));
	}
	else
	{
		UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
		SelfNode->AllocateDefaultPins();

		// Dummy connection to self (It needs to be connected to something)
		Schema->TryCreateConnection(SelfNode->FindPinChecked(UEdGraphSchema_K2::PN_Self), ModifierNode->FindPinChecked(TEXT("PlayerController")));
	}
	
	UEdGraphPin* NodeExec = GetExecPin();
	UEdGraphPin* NodeThen = FindPin(UEdGraphSchema_K2::PN_Then);
	UEdGraphPin* InternalExec = ModifierNode->GetExecPin();
	CompilerContext.MovePinLinksToIntermediate(*NodeExec, *InternalExec);
	UEdGraphPin* InternalThen = ModifierNode->GetThenPin();
	CompilerContext.MovePinLinksToIntermediate(*NodeThen, *InternalThen);
	
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(ValuePinName), *ModifierNode->FindPinChecked(ValuePinName));
}

void UK2Node_SetEnhancedSettingValue::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	auto CustomizeEnhancedSettingNodeLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode, TWeakObjectPtr<const UEnhancedSetting> Setting)
	{
		UK2Node_SetEnhancedSettingValue* InputNode = CastChecked<UK2Node_SetEnhancedSettingValue>(NewNode);
		InputNode->Initialize(Setting.Get());
	};
	
	// Do a first time registration using the node's class to pull in all existing actions
	if (ActionRegistrar.IsOpenForRegistration(GetClass()))
	{
		IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

		static bool bRegisterOnce = true;
		if (bRegisterOnce)
		{
			bRegisterOnce = false;
			if (AssetRegistry.IsLoadingAssets())
			{
				AssetRegistry.OnFilesLoaded().AddLambda([]() { FBlueprintActionDatabase::Get().RefreshClassActions(StaticClass()); });
			}
		}
		
		TArray<FAssetData> SettingAssets;
		AssetRegistry.GetAssetsByClass(UEnhancedSetting::StaticClass()->GetClassPathName(), SettingAssets, true);
		for (const FAssetData& SettingAsset : SettingAssets)
		{
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
			check(NodeSpawner != nullptr);

			if (FPackageName::GetPackageMountPoint(SettingAsset.PackageName.ToString()) != NAME_None)
			{
				if (const UEnhancedSetting* Setting = Cast<const UEnhancedSetting>(SettingAsset.GetAsset()))
				{
					NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeEnhancedSettingNodeLambda, TWeakObjectPtr<const UEnhancedSetting>(Setting));
					ActionRegistrar.AddBlueprintAction(Setting, NodeSpawner);
				}
			}
		}
	}
	else if (const UEnhancedSetting* Setting = Cast<const UEnhancedSetting>(ActionRegistrar.GetActionKeyFilter()))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeEnhancedSettingNodeLambda, TWeakObjectPtr<const UEnhancedSetting>(Setting));
		ActionRegistrar.AddBlueprintAction(Setting, NodeSpawner);
	}
}

FText UK2Node_SetEnhancedSettingValue::GetMenuCategory() const
{
	static FNodeTextCache CachedCategory;
	if (CachedCategory.IsOutOfDate(this))
	{
		// Directly use a custom category string
		CachedCategory.SetCachedText(LOCTEXT("EnhancedSettingMenuCategory", "Enhanced Settings|Values|Set"), this);
	}
	return CachedCategory;
};

void UK2Node_SetEnhancedSettingValue::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	//Super::ValidateNodeDuringCompilation(MessageLog);
	
	if (!EnhancedSetting)
	{
		MessageLog.Error(*LOCTEXT("EnhancedSetting_ErrorFmt", "EnhancedEnhancedSettingEvent references invalid 'null' setting asset for @@").ToString(), this);
	}
}

bool UK2Node_SetEnhancedSettingValue::IsCompatibleWithGraph(UEdGraph const* Graph) const
{
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);

	UEdGraphSchema_K2 const* K2Schema = Cast<UEdGraphSchema_K2>(Graph->GetSchema());
	bool const bIsConstructionScript = (K2Schema != nullptr) ? K2Schema->IsConstructionScript(Graph) : false;

	return (Blueprint != nullptr) && Blueprint->SupportsInputEvents() && !bIsConstructionScript && Super::IsCompatibleWithGraph(Graph);
}

FBlueprintNodeSignature UK2Node_SetEnhancedSettingValue::GetSignature() const
{
	FBlueprintNodeSignature NodeSignature = Super::GetSignature();
	NodeSignature.AddKeyValue(GetEnhancedSettingName(EnhancedSetting).ToString());

	return NodeSignature;
}

UObject* UK2Node_SetEnhancedSettingValue::GetJumpTargetForDoubleClick() const
{
	return const_cast<UObject*>(Cast<UObject>(EnhancedSetting));
}

void UK2Node_SetEnhancedSettingValue::JumpToDefinition() const
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(GetJumpTargetForDoubleClick());
}

FText UK2Node_SetEnhancedSettingValue::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return FText::FromName(GetEnhancedSettingName(EnhancedSetting));
	}
	else if (CachedNodeTitle.IsOutOfDate(this))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("EnhancedSettingName"), FText::FromName(GetEnhancedSettingName(EnhancedSetting)));

		FText LocFormat = LOCTEXT("EnhancedSetting_Name", "Change {EnhancedSettingName}");
		// FText::Format() is slow, so we cache this to save on performance
		CachedNodeTitle.SetCachedText(FText::Format(LocFormat, Args), this);
	}

	return CachedNodeTitle;
}

FText UK2Node_SetEnhancedSettingValue::GetTooltipText() const
{
	if (CachedTooltip.IsOutOfDate(this))
	{
		// FText::Format() is slow, so we cache this to save on performance
		FString ActionPath = EnhancedSetting ? EnhancedSetting->GetFullName() : TEXT("");
		CachedTooltip.SetCachedText(
			FText::Format(LOCTEXT("ChangeEnhancedSetting_Tooltip", "Change the current SETTING value of {0}. You will still need to apply changes for them to be saved (Committed). \n\nNote: If the value is not accessible or modifiable due to edit conditions not being met this call will not create a pending change."),
			FText::FromString(ActionPath)), this);
	}
	return CachedTooltip;
}

FLinearColor UK2Node_SetEnhancedSettingValue::GetNodeTitleColor() const
{
	return  NodeDefaultColor;
}

#undef LOCTEXT_NAMESPACE
