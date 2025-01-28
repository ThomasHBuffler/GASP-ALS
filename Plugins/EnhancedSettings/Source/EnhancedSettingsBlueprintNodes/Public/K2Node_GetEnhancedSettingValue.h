// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraphNodeUtils.h"
#include "K2Node.h"
#include "K2Node_GetEnhancedSettingValue.generated.h"

class UEnhancedSetting;
namespace ENodeTitleType { enum Type : int; }
struct FBlueprintNodeSignature;

class FBlueprintActionDatabaseRegistrar;
class UDynamicBlueprintBinding;

UCLASS(meta=(Keywords = "Get, Setting"))
class ENHANCEDSETTINGSBLUEPRINTNODES_API UK2Node_GetEnhancedSettingValue : public UK2Node
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	TObjectPtr<const UEnhancedSetting> EnhancedSetting;

	//~ Begin EdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool IsCompatibleWithGraph(UEdGraph const* Graph) const override;
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	virtual void JumpToDefinition() const override;
	//~ End EdGraphNode Interface

	//~ Begin UK2Node Interface
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual FBlueprintNodeSignature GetSignature() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual bool IsNodePure() const override { return true; }
	//~ End UK2Node Interface

	void Initialize(const UEnhancedSetting* InSetting);
	
	/*
	// Auto pin generation helpers
	static FName GetValueCategory(const UEnhancedSetting* InSetting);
	static FName GetValueSubCategory(const UEnhancedSetting* InSetting);
	static UScriptStruct* GetValueSubCategoryObject(const UEnhancedSetting* InSetting);
	*/
	
private:
	// Constructing FText strings can be costly, so we cache the node's title/tooltip 
	FNodeTextCache CachedTooltip;
	FNodeTextCache CachedNodeTitle;
};

#if UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2
#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "InputAction.h"
#endif
