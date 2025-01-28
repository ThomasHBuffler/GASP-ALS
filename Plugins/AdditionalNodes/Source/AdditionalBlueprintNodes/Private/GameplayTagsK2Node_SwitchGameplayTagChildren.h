// Copyright Epic Games, Inc. All Rights Reserved.


#pragma once

#include "K2Node_Switch.h"
#include "GameplayTagContainer.h"
#include "NodeMetadata.h"
#include "GameplayTagsK2Node_SwitchGameplayTagChildren.generated.h"

namespace ENodeTitleType { enum Type : int; }

class FBlueprintActionDatabaseRegistrar;

UCLASS(MinimalAPI)
class UGameplayTagsK2Node_SwitchGameplayTagChildren : public UK2Node_Switch
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FGameplayTag> PinTags;

	UPROPERTY(EditAnywhere, Category = PinOptions)
	bool bCallParentNode;
	
	UPROPERTY(EditAnywhere, Category = PinOptions)
	bool bDirectChildrenOnly;

	
	UPROPERTY()
	TArray<FName> PinNames;


	// UObject interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
	// End of UObject interface

	// UEdGraphNode interface
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// Color
	virtual FLinearColor GetNodeTitleColor() const override { return NodeDefaultColor; }
	virtual bool ShouldShowNodeProperties() const override { return true; }
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End of UK2Node interface

	// UK2Node_Switch Interface
	ADDITIONALBLUEPRINTNODES_API virtual void AddPinToSwitchNode() override;
	virtual FName GetUniquePinName() override;
	virtual FEdGraphPinType GetPinType() const override;
	ADDITIONALBLUEPRINTNODES_API virtual FEdGraphPinType GetInnerCaseType() const override;
	// End of UK2Node_Switch Interface

	virtual FName GetPinNameGivenIndex(int32 Index) const override;

protected:
	virtual void CreateFunctionPin() override;
	virtual void CreateSelectionPin() override;
	virtual void CreateCasePins() override;
	virtual void RemovePin(UEdGraphPin* TargetPin) override;
};

#if UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2
#include "CoreMinimal.h"
#endif
