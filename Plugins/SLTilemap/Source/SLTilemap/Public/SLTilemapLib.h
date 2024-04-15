// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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





FORCEINLINE int32 inline_IndexToX(const int32 Index, const int32 Width)
{
    return Index % Width;
}

FORCEINLINE int32 inline_IndexToY(const int32 Index, const int32 Width)
{
    return Index / Width;
}

FORCEINLINE int32 inline_XYToIndex(const int32 X, const int32 Y, const int32 Width)
{
    return Y * Width + X;
}

FORCEINLINE int32 inline_IndexToIndexReflected(const int32 Index, const int32 Width)
{
    const int32 X = inline_IndexToX(Index, Width);
    const int32 Y = inline_IndexToY(Index, Width);
    const int32 XReflected = Width - X - 1;
    const int32 YReflected = Y;
    const int32 WidthReflected = Width;
    const int32 IndexReflected = inline_XYToIndex(XReflected, YReflected, WidthReflected);
    return IndexReflected;
}

FORCEINLINE int32 inline_IndexToIndexTransposed(const int32 Index, const int32 Width, const int32 Height)
{
    const int32 X = inline_IndexToX(Index, Width);
    const int32 Y = inline_IndexToY(Index, Width);
    const int32 XTransposed = Y;
    const int32 YTransposed = X;
    const int32 WidthTransposed = Height;
    const int32 IndexTransposed = inline_XYToIndex(XTransposed, YTransposed, WidthTransposed);
    return IndexTransposed;
}

FORCEINLINE int32 inline_IndexToIndexRotated(const int32 Index, const int32 Width, const int32 Height)
{
    const int32 X = inline_IndexToX(Index, Width);
    const int32 Y = inline_IndexToY(Index, Width);
    const int32 XRotated = Height - Y - 1;
    const int32 YRotated = X;
    const int32 WidthRotated = Height;
    const int32 IndexTransposed = inline_XYToIndex(XRotated, YRotated, WidthRotated);
    return IndexTransposed;
}





USTRUCT(BlueprintType)
struct FTilePattern
{
    GENERATED_BODY()
    uint8 Data[9] = {};

    static FTilePattern Reflect(const FTilePattern In)
    {
        FTilePattern Result;
        for (int i = 0; i < 9; i++)
        {
            const int32 Index = inline_IndexToIndexReflected(i, 3);
            Result.Data[i] = In.Data[Index];
        }
        return Result;
    }

    static FTilePattern Transpose(const FTilePattern In)
    {
        FTilePattern Result;
        for (int i = 0; i < 9; i++)
        {
            const int32 Index = inline_IndexToIndexTransposed(i, 3, 3);
            Result.Data[i] = In.Data[Index];
        }
        return Result;
    }

    static FTilePattern Rotate(const FTilePattern In)
    {
        FTilePattern Result;
        for (int i = 0; i < 9; i++)
        {
            const int32 Index = inline_IndexToIndexRotated(i, 3, 3);
            Result.Data[i] = In.Data[Index];
        }
        return Result;
    }

    static FTilePattern And(const FTilePattern A, const FTilePattern B)
    {
        FTilePattern Result;
        for (int i = 0; i < 9; i++)
        {
            Result.Data[i] = A.Data[i] & B.Data[i];
        }
        return Result;
    }

    static FTilePattern Or(const FTilePattern A, const FTilePattern B)
    {
        FTilePattern Result;
        for (int i = 0; i < 9; i++)
        {
            Result.Data[i] = A.Data[i] | B.Data[i];
        }
        return Result;
    }
};

FORCEINLINE bool operator ==(const FTilePattern& A, const FTilePattern& B)
{
    for (int i = 0; i < 9; i++)
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
        Width = 3;
        Height = 3;
        Data.Init(0, Width * Height);
    }

    FTileMap(const int32 NewWidth, const int32 NewHeight)
    {
        Width = NewWidth;
        Height = NewHeight;
        Data.Init(0, Width * Height);
    }

    FTileMap(const int32 NewWidth, const int32 NewHeight, const uint8 InitialValue)
    {
        Width = NewWidth;
        Height = NewHeight;
        Data.Init(InitialValue, Width * Height);
    }

    FTileMap(const int32 NewWidth, const int32 NewHeight, const TArray<uint8>& NewData)
    {
        Width = NewWidth;
        Height = NewHeight;
        Data = NewData;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
    int32 Width;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
    int32 Height;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap", meta = (Bitmask, BitmaskEnum = "ETileState"))
    TArray<uint8> Data;

    uint8 GetTile(const int32 X, const int32 Y) const
    {
        return Data[inline_XYToIndex(X, Y, Width)];
    }

    void SetTile(const uint8 Tile, const int32 X, const int32 Y)
    {
        Data[inline_XYToIndex(X, Y, Width)] = Tile;
    }
    
    static FTileMap Reflect(const FTileMap In)
    {
        FTileMap Result = FTileMap(In.Width, In.Height);
        for (int i = 0; i < In.Data.Num(); i++)
        {
            const int32 Index = inline_IndexToIndexReflected(i, In.Width);
            Result.Data[i] = In.Data[Index];
        }
        return Result;
    }

    static FTileMap Transpose(const FTileMap In)
    {
        FTileMap Result = FTileMap(In.Height, In.Width);
        for (int i = 0; i < In.Data.Num(); i++)
        {
            const int32 Index = inline_IndexToIndexTransposed(i, In.Height, In.Width);
            Result.Data[i] = In.Data[Index];
        }
        return Result;
    }

    static FTileMap Rotate(const FTileMap In)
    {
        FTileMap Result = FTileMap(In.Height, In.Width);
        for (int i = 0; i < In.Data.Num(); i++)
        {
            const int32 Index = inline_IndexToIndexRotated(i, In.Height, In.Width);
            Result.Data[i] = In.Data[Index];
        }
        return Result;
    }

    FTilePattern ReadPattern(const int32 StartX, const int32 StartY) const
    {
        FTilePattern Result = FTilePattern();
        for (int32 Y = 0; Y < 3; Y++)
        {
            for (int32 X = 0; X < 3; X++)
            {
                const uint8 Tile = GetTile(X + StartX, Y + StartY);
                Result.Data[inline_XYToIndex(X, Y, 3)] = Tile;
            }
        }
        return Result;
    }

    void WritePattern(const FTilePattern Pattern, const int32 StartX, const int32 StartY)
    {
        for (int32 Y = 0; Y < 3; Y++)
        {
            for (int32 X = 0; X < 3; X++)
            {
                const int32 PatternIndex = inline_XYToIndex(X, Y, 3);
                const uint8 Tile = Pattern.Data[PatternIndex];
                SetTile(Tile, X + StartX, Y + StartY);
            }
        }
    }
};

FORCEINLINE bool operator ==(const FTileMap& A, const FTileMap& B)
{
    return A.Width == B.Width && A.Data == B.Data;
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
    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static void RunTests();
    
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static int32 XYToIndex(const int32 X, const int32 Y, const int32 Width);

    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static FTileMap CreateTileMap(const int32 NewWidth, const int32 NewHeight, const uint8 InitialValue);
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static bool IsTilemapValid(const FTileMap& TileMap);

    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static uint8 GetTileAtXY(const FTileMap& TileMap, const int32 X, const int32 Y);
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static FTileMap GetTilemapSection(const FTileMap& TileMap, const int32 X, const int32 Y, const int32 SectionWidth, const int32 SectionHeight);

    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static void SetTileAtXY(UPARAM(ref) FTileMap& TileMap, const uint8 Tile, const int32 X, const int32 Y);
    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static void SetBorder(UPARAM(ref) FTileMap& TileMap, const uint8 Tile);
    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static void FloodFill(UPARAM(ref) FTileMap& TileMap, const int32 X, const int32 Y, const uint8 Tile, const uint8 TileToReplace);

    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static FTileMap ReflectTilemap(const FTileMap& TileMap);
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static FTileMap TransposeTilemap(const FTileMap& TileMap);
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static FTileMap RotateTilemap(const FTileMap& TileMap);

    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static UTexture2D* TileMapToTexture(UPARAM(ref) FTileMap& TileMap);
    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static FColor TileToColor(const uint8 Tile);
};
