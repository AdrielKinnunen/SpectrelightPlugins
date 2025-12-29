// Copyright Spectrelight Studios, LLC


#include "SLTilemapDataAsset.h"

#if WITH_EDITOR
void USLTilemapDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	Tilemap.Size.X = FMath::Max(Tilemap.Size.X, 3);
	Tilemap.Size.Y = FMath::Max(Tilemap.Size.Y, 3);
	const int32 ExpectedSize = Tilemap.Size.X*Tilemap.Size.Y;
	if (Tilemap.Data.Num()!=ExpectedSize)
	{
		Tilemap.Data.SetNum(ExpectedSize);
	}
}
#endif
