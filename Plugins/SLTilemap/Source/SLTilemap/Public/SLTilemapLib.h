// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLTilemapLib.generated.h"

UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ETileState : uint8
{
	None			= 0 UMETA(Hidden),
	Void			= 1<<0,
	Ground			= 1<<1,
	Wall			= 1<<2,
	Window			= 1<<3,
	RoofedVoid		= 1<<4,
	RoofedGround	= 1<<5,
	RoofedWall		= 1<<6,
	RoofedWindow	= 1<<7
};
ENUM_CLASS_FLAGS(ETileState);

USTRUCT(BlueprintType)
struct FTileMap
{
	GENERATED_BODY()
	FTileMap()
	{
		SizeX = 3;
		SizeY = 3;
		Data.Init(0, SizeX*SizeY);
	}

	FTileMap(const int32 NewSizeX, const int32 NewSizeY)
	{
		SizeX = NewSizeX;
		SizeY = NewSizeY;
		Data.Init(0, SizeX*SizeY);
	}

	FTileMap(const int32 NewSizeX, const int32 NewSizeY, const uint8 InitialValue)
	{
		SizeX = NewSizeX;
		SizeY = NewSizeY;
		Data.Init(InitialValue, SizeX*SizeY);
	}

	FTileMap(const int32 NewSizeX, const int32 NewSizeY, const TArray<uint8> NewData)
	{
		SizeX = NewSizeX;
		SizeY = NewSizeY;
		Data = NewData;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 SizeX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 SizeY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap", meta = (Bitmask, BitmaskEnum = "ETileState"))
	TArray<uint8> Data;


	int32 GetX(const int32 Index) const
	{
		return Index % SizeX;
	}
	
	int32 GetY(const int32 Index) const
	{
		return Index / SizeX;
	}

	int32 GetIndex(const int32 X, const int32 Y) const
	{
		return Y * SizeX + X;
	}

	uint8 GetTile(const int32 X, const int32 Y) const
	{
		return Data[Y * SizeX + X];
	}

	void SetTile(const uint8 Tile, const int32 X, const int32 Y)
	{
		Data[Y * SizeX + X] = Tile;
	}
};

FORCEINLINE bool operator ==(const FTileMap& A, const FTileMap& B)
{
	return A.SizeX == B.SizeX && A.Data == B.Data;
}



USTRUCT(BlueprintType)
struct FTileMapInfo
{
	GENERATED_BODY()
	FTileMapInfo()
	{
		//TileMap = FTileMap();
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	FTileMap TileMap;
};









UCLASS()
class SLTILEMAP_API USLTilemapLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static int32 XYToIndex(const int32 SizeX, const int32 X, const int32 Y);
	
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static FTileMap CreateTileMap(const int32 NewSizeX, const int32 NewSizeY, const uint8 InitialValue);
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static bool IsTilemapValid(const FTileMap& TileMap);

	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static uint8 GetTileAtXY(const FTileMap& TileMap, const int32 X, const int32 Y);
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static FTileMap GetTilemapSection(const FTileMap& TileMap, const int32 X, const int32 Y, const int32 SectionSizeX, const int32 SectionSizeY);
	
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static void SetTileAtXY(UPARAM(ref) FTileMap& TileMap, const uint8 Tile, const int32 X, const int32 Y);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static void SetBorder(UPARAM(ref) FTileMap& TileMap, const uint8 Tile);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static void FloodFill(UPARAM(ref) FTileMap& TileMap, const int32 X, const int32 Y, const uint8 Tile, const uint8 TileToReplace);

	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static FTileMap MirrorTilemap(const FTileMap& TileMap);
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static FTileMap RotateTilemap(const FTileMap& TileMap);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static UTexture2D* TileMapToTexture(UPARAM(ref) FTileMap& TileMap);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static FColor TileToColor(const uint8 Tile);
	
};
