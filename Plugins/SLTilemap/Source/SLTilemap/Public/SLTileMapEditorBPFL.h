// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLTilemapEditorBPFL.generated.h"

/**
 * 
 */
UCLASS()
class SLTILEMAP_API USLTilemapEditorBPFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	//UFUNCTION(BlueprintCallable, Category = "SLTileMap", Meta = (WorldContext="WorldContextObject"))
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static bool GetMouseVectorInViewport(FVector& OutWorldOrigin, FVector& OutWorldDirection);
};
