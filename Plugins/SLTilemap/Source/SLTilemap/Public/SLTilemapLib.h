// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLTilemapLib.generated.h"


UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ETileState : uint8
{
    None = 0 UMETA(Hidden),
    Void = 1 << 0,
    Ground = 1 << 1,
    Wall = 1 << 2,
    Window = 1 << 3,
    RoofedVoid = 1 << 4,
    RoofedGround = 1 << 5,
    RoofedWall = 1 << 6,
    RoofedWindow = 1 << 7
};
ENUM_CLASS_FLAGS(ETileState);





FORCEINLINE int32 inline_IndexToX(const int32 Index, const int32 SizeX)
{
    return Index % SizeX;
}

FORCEINLINE int32 inline_IndexToY(const int32 Index, const int32 SizeX)
{
    return Index / SizeX;
}

FORCEINLINE int32 inline_XYToIndex(const int32 X, const int32 Y, const int32 SizeX)
{
    return Y * SizeX + X;
}





USTRUCT(BlueprintType)
struct FTileKernel
{
    GENERATED_BODY()
    uint8 Data[16];

    static FTileKernel And(const FTileKernel A, const FTileKernel B)
    {
        FTileKernel Result;
        for (int i = 0; i < 16; i++)
        {
            Result.Data[i] = A.Data[i] & B.Data[i];
        }
        return Result;
    }

    static FTileKernel Or(const FTileKernel A, const FTileKernel B)
    {
        FTileKernel Result;
        for (int i = 0; i < 16; i++)
        {
            Result.Data[i] = A.Data[i] | B.Data[i];
        }
        return Result;
    }
};

FORCEINLINE bool operator ==(const FTileKernel& A, const FTileKernel& B)
{
    for (int i = 0; i < 16; i++)
    {
        if (A.Data[i] != B.Data[i])
        {
            return false;
        }
    }
    return true; 
}





USTRUCT(BlueprintType)
struct FTileMap
{
    GENERATED_BODY()
    FTileMap()
    {
        SizeX = 3;
        SizeY = 3;
        Data.Init(0, SizeX * SizeY);
    }

    FTileMap(const int32 NewSizeX, const int32 NewSizeY)
    {
        SizeX = NewSizeX;
        SizeY = NewSizeY;
        Data.Init(0, SizeX * SizeY);
    }

    FTileMap(const int32 NewSizeX, const int32 NewSizeY, const uint8 InitialValue)
    {
        SizeX = NewSizeX;
        SizeY = NewSizeY;
        Data.Init(InitialValue, SizeX * SizeY);
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

    uint8 GetTile(const int32 X, const int32 Y) const
    {
        return Data[inline_XYToIndex(X, Y, SizeX)];
    }

    void SetTile(const uint8 Tile, const int32 X, const int32 Y)
    {
        Data[inline_XYToIndex(X, Y, SizeX)] = Tile;
    }

    FTileKernel ReadKernel(const int32 KernelSize, const int32 StartX, const int32 StartY) const
    {
        FTileKernel Result = FTileKernel();
        for (int32 Y = 0; Y < KernelSize; Y++)
        {
            for (int32 X = 0; X < KernelSize; X++)
            {
                const uint8 Tile = GetTile(X + StartX, Y + StartY);
                Result.Data[inline_XYToIndex(X, Y, KernelSize)] = Tile;
            }
        }
        return Result;
    }

    void WriteKernel(const FTileKernel Kernel, const int32 KernelSize, const int32 StartX, const int32 StartY)
    {
        for (int32 Y = 0; Y < KernelSize; Y++)
        {
            for (int32 X = 0; X < KernelSize; X++)
            {
                const int32 KernelIndex = inline_XYToIndex(X, Y, KernelSize);
                const uint8 Tile = Kernel.Data[KernelIndex];
                SetTile(Tile, X + StartX, Y + StartY);
            }
        }
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
    static int32 XYToIndex(const int32 X, const int32 Y, const int32 SizeX);

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
