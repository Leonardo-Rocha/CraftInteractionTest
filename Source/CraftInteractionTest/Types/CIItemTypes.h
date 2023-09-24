#include <CoreMinimal.h>
#include <Engine/DataTable.h>

#include "CIItemTypes.generated.h"

class ACIItemInstance;

USTRUCT(BlueprintType)
struct FItemDefinition : public FTableRowBase
{
	GENERATED_BODY()

public:
	FItemDefinition() {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSoftClassPtr<ACIItemInstance> ItemInstanceClass;
};

USTRUCT(BlueprintType)
struct FRecipeIntegredient
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FDataTableRowHandle Item;

	// TODO: to be supported later
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	int32 Quantity = 1;*/
};

USTRUCT(BlueprintType)
struct FRecipe : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FDataTableRowHandle ResultItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	TArray<FRecipeIntegredient> Ingredients;
};
