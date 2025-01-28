// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_GetEnhancedSettingValue.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "Editor.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "EnhancedSetting.h"
#include "K2Node_EnhancedSettingValueAccessor.h"
#include "K2Node_Self.h"
#include "Misc/PackageName.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Modules/ModuleManager.h"
#include "EnhancedSettingsTypes.h"
#include "NodeMetadata.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_GetEnhancedSettingValue)

#define LOCTEXT_NAMESPACE "K2Node_GetEnhancedSettingValue"

 
// @TODO INPUT REMOVE INPUT REMOVE TODO INPUT REMOVE INPUT REMOVE TODO INPUT REMOVE INPUT REMOVE TODO INPUT REMOVE INPUT REMOVE

UK2Node_GetEnhancedSettingValue::UK2Node_GetEnhancedSettingValue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UK2Node_GetEnhancedSettingValue::AllocateDefaultPins()
{
	PreloadObject(static_cast<UObject*>(EnhancedSetting));

	Super::AllocateDefaultPins();
	
	if (!IsValid(EnhancedSetting)) return;
	
	if (EnhancedSetting->GetEnhancedSettingScope() == LocalPlayer)
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, APlayerController::StaticClass(), TEXT("PlayerController"));
	}
	
	// Dynamically typed output
	FName SubCategory = GetValueSubCategory(EnhancedSetting);

	if (SubCategory != NAME_None)
	{
		CreatePin(EGPD_Output, GetValueCategory(EnhancedSetting), SubCategory, UEdGraphSchema_K2::PN_ReturnValue);
	}
	else
	{
		CreatePin(EGPD_Output, GetValueCategory(EnhancedSetting), GetValueSubCategoryObject(EnhancedSetting), UEdGraphSchema_K2::PN_ReturnValue);
	}
}

void UK2Node_GetEnhancedSettingValue::Initialize(const UEnhancedSetting* InSetting)
{
	// LOG THE FNAME OF THE SETTING AS WELL AS THIS CLASS
	EnhancedSetting = InSetting;
}

void UK2Node_GetEnhancedSettingValue::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	if (!EnhancedSetting) return;	
	
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

	UEdGraphPin* ReturnValue = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	if (ReturnValue->LinkedTo.Num() == 0)
	{
		return;
	}
	
	// Accessor does the call to the function
	UK2Node_EnhancedSettingValueAccessor* AccessorNode = CompilerContext.SpawnIntermediateNode<UK2Node_EnhancedSettingValueAccessor>(this, SourceGraph);
	AccessorNode->Initialize(EnhancedSetting);
	AccessorNode->AllocateDefaultPins();
	
	if (EnhancedSetting->GetEnhancedSettingScope() == LocalPlayer)
	{
		UEdGraphPin* PCPinin = FindPinChecked(TEXT("PlayerController"));
		
		CompilerContext.MovePinLinksToIntermediate(*PCPinin, *AccessorNode->FindPinChecked(TEXT("PlayerController")));
	}
	else
	{
		UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
		SelfNode->AllocateDefaultPins();
		
		// Dummy connection to self (It needs to be connected to something)
		Schema->TryCreateConnection(SelfNode->FindPinChecked(UEdGraphSchema_K2::PN_Self), AccessorNode->FindPinChecked(TEXT("PlayerController")));
	}

	// And finally hook the EnhancedSettingValueAccessor's return value up to the GetEnhancedSettingValue return pin's connected nodes.
	
	TArray<UEdGraphPin*> LinkedTo(ReturnValue->LinkedTo);
	for (UEdGraphPin* EachLink : LinkedTo)
	{
		Schema->TryCreateConnection(AccessorNode->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue), EachLink);
	}
}

void UK2Node_GetEnhancedSettingValue::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	auto CustomizeEnhancedSettingNodeLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode, TWeakObjectPtr<const UEnhancedSetting> Setting)
	{
		UK2Node_GetEnhancedSettingValue* InputNode = CastChecked<UK2Node_GetEnhancedSettingValue>(NewNode);
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
		// If this is a specific UEnhancedSetting asset update it.
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeEnhancedSettingNodeLambda, TWeakObjectPtr<const UEnhancedSetting>(Setting));
		ActionRegistrar.AddBlueprintAction(Setting, NodeSpawner);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("UK2Node_GetEnhancedSettingValue: GetMenuActions: Setting is NULL AND WILL ALWAYS BE NULL"));
	}
}

FText UK2Node_GetEnhancedSettingValue::GetMenuCategory() const
{
	static FNodeTextCache CachedCategory;
	if (CachedCategory.IsOutOfDate(this))
	{
		// FText::Format() is slow, so we cache this to save on performance
		CachedCategory.SetCachedText(LOCTEXT("EnhancedSettingMenuCategory", "Enhanced Settings|Values|Get"), this);
	}
	return CachedCategory;
}

void UK2Node_GetEnhancedSettingValue::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	if (!EnhancedSetting)
	{
		MessageLog.Error(*LOCTEXT("EnhancedEnhancedSetting_ErrorFmt", "EnhancedEnhancedSettingEvent references invalid 'null' setting asset for @@").ToString(), this);
	}
}

bool UK2Node_GetEnhancedSettingValue::IsCompatibleWithGraph(UEdGraph const* Graph) const
{
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);

	UEdGraphSchema_K2 const* K2Schema = Cast<UEdGraphSchema_K2>(Graph->GetSchema());
	bool const bIsConstructionScript = (K2Schema != nullptr) ? K2Schema->IsConstructionScript(Graph) : false;

	return (Blueprint != nullptr) && Blueprint->SupportsInputEvents() && !bIsConstructionScript && Super::IsCompatibleWithGraph(Graph);
}

FBlueprintNodeSignature UK2Node_GetEnhancedSettingValue::GetSignature() const
{
	FBlueprintNodeSignature NodeSignature = Super::GetSignature();
	NodeSignature.AddKeyValue(GetEnhancedSettingName(EnhancedSetting).ToString());

	return NodeSignature;
}

UObject* UK2Node_GetEnhancedSettingValue::GetJumpTargetForDoubleClick() const
{
	return const_cast<UObject*>(Cast<UObject>(EnhancedSetting));
}

void UK2Node_GetEnhancedSettingValue::JumpToDefinition() const
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(GetJumpTargetForDoubleClick());
}

FText UK2Node_GetEnhancedSettingValue::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{	
		return FText::FromName(GetEnhancedSettingName(EnhancedSetting));
	}
	else if (CachedNodeTitle.IsOutOfDate(this))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("EnhancedSettingName"), FText::FromName(GetEnhancedSettingName(EnhancedSetting)));

		FText LocFormat = LOCTEXT("GetEnhancedSetting_Name", "Get {EnhancedSettingName}");
		// FText::Format() is slow, so we cache this to save on performance
		CachedNodeTitle.SetCachedText(FText::Format(LocFormat, Args), this);
	}

	return CachedNodeTitle;
}

FText UK2Node_GetEnhancedSettingValue::GetTooltipText() const
{
	if (CachedTooltip.IsOutOfDate(this))
	{
		// FText::Format() is slow, so we cache this to save on performance
		FString ActionPath = EnhancedSetting ? EnhancedSetting->GetFullName() : TEXT("");
		CachedTooltip.SetCachedText(
			FText::Format(LOCTEXT("GetEnhancedSetting_Tooltip", "Returns the current SETTING value of {0}.  Ensure the bindings where registered for this class or this will return 0. \n\nNote: If the value is not accessible or modifiable due to edit conditions not being met this will return 0."),
			FText::FromString(ActionPath)), this);
	}
	return CachedTooltip;
}

FLinearColor UK2Node_GetEnhancedSettingValue::GetNodeTitleColor() const
{
	return NodeDefaultColor;
}
#undef LOCTEXT_NAMESPACE

