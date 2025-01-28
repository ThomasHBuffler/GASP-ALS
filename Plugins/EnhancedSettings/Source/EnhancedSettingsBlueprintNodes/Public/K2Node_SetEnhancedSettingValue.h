// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "K2Node_SetEnhancedSettingValue.generated.h"

class UEnhancedSetting;
/**
 * 
 */
UCLASS()
class ENHANCEDSETTINGSBLUEPRINTNODES_API UK2Node_SetEnhancedSettingValue : public UK2Node_CallFunction
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<const UEnhancedSetting> EnhancedSetting;

public:	
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
	virtual bool IsNodePure() const override { return false; }
	//~ End UK2Node Interface
	
	void Initialize(const UEnhancedSetting* InSetting);

private:
	// Constructing FText strings can be costly, so we cache the node's title/tooltip 
	FNodeTextCache CachedTooltip;
	FNodeTextCache CachedNodeTitle;
};
