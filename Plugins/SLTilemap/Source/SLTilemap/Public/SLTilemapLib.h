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



USTRUCT(BlueprintType)
struct FTileMapCoords
{
	GENERATED_BODY()
	FTileMapCoords()
	{
		X = 0;
		Y = 0;
	}
	FTileMapCoords(const int32 NewX, const int32 NewY)
	{
		X = NewX;
		Y = NewY;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 Y;
	
	bool operator== (const FTileMapCoords& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}
	
	friend uint32 GetTypeHash (const FTileMapCoords& Other)
	{
		return HashCombineFast(GetTypeHash(Other.X), GetTypeHash(Other.Y));
	}
};



FORCEINLINE int32 inline_CoordsToIndex(const FTileMapCoords Coords, const FTileMapCoords Size)
{
	return Coords.Y * Size.X + Coords.X;
}

FORCEINLINE FTileMapCoords inline_IndexToCoords(const int32 Index, const FTileMapCoords Size)
{
	return FTileMapCoords(Index % Size.X, Index / Size.X);
}


FORCEINLINE FColor inline_TileToColor(const uint8 Tile)
{
	const ETileState Flags = static_cast<ETileState>(Tile);
	const uint8 Void = EnumHasAnyFlags(Flags, ETileState::Void | ETileState::RoofedVoid);
	const uint8 Ground = EnumHasAnyFlags(Flags, ETileState::Ground | ETileState::RoofedGround);
	const uint8 Wall = EnumHasAnyFlags(Flags, ETileState::Wall | ETileState::RoofedWall);
	const uint8 Window = EnumHasAnyFlags(Flags, ETileState::Window | ETileState::RoofedWindow);
	const uint8 Roofed = EnumHasAnyFlags(Flags, ETileState::RoofedVoid | ETileState::RoofedGround | ETileState::RoofedWall | ETileState::RoofedWindow);
	const uint8 IsPow2 = (Tile & (Tile - 1)) == 0;
	/*
	const uint8 R = (6 * IsPow2 + 2) * (Wall * 16 + Window * 16);
	const uint8 G = (6 * IsPow2 + 2) * (Window * 16 + Ground * 16);
	const uint8 B = (6 * IsPow2 + 2) * (Void * 16 + Ground * 8 + Window * 8);
	const uint8 A = 255 * Roofed;
	*/

	uint8 R = (6 * IsPow2 + 2) * (Wall * 16 + Window * 16);
	uint8 G = (6 * IsPow2 + 2) * (Window * 16 + Ground * 16);
	uint8 B = (6 * IsPow2 + 2) * (Void * 16 + Ground * 8 + Window * 8);
	const uint8 A = 255 * Roofed;
	R *= IsPow2;
	G *= IsPow2;
	B *= IsPow2;
    
	return FColor(R, G, B, A);
}



USTRUCT(BlueprintType)
struct FTilePattern
{
    GENERATED_BODY()
    uint8 Data[9] = {};
	
	friend uint32 GetTypeHash (const FTilePattern& Other)
	{
		return GetArrayHash(Other.Data, 9);
		//return GetTypeHash(Other.Data[0]);
	}
	
	void Reflect()
	{
		uint8 Result[9];
		Result[0] = Data[2];
		Result[1] = Data[1];
		Result[2] = Data[0];
		Result[3] = Data[5];
		Result[4] = Data[4];
		Result[5] = Data[3];
		Result[6] = Data[8];
		Result[7] = Data[7];
		Result[8] = Data[6];
		for (int i = 0; i < 9; i++)
		{
			Data[i] = Result[i];
		}
	}

	void Transpose()
	{
		uint8 Result[9];
		Result[0] = Data[0];
		Result[1] = Data[3];
		Result[2] = Data[6];
		Result[3] = Data[1];
		Result[4] = Data[4];
		Result[5] = Data[7];
		Result[6] = Data[2];
		Result[7] = Data[5];
		Result[8] = Data[8];
		for (int i = 0; i < 9; i++)
		{
			Data[i] = Result[i];
		}
	}

	void Rotate()
	{
		uint8 Result[9];
		Result[0] = Data[2];
		Result[1] = Data[5];
		Result[2] = Data[8];
		Result[3] = Data[1];
		Result[4] = Data[4];
		Result[5] = Data[7];
		Result[6] = Data[0];
		Result[7] = Data[3];
		Result[8] = Data[6];
		for (int i = 0; i < 9; i++)
		{
			Data[i] = Result[i];
		}
	}
	
	void AndWith(FTilePattern Other)
	{
		for (int i = 0; i < 9; i++)
		{
			Data[i] = Data[i] & Other.Data[i];
		}
	}

	void OrWith(FTilePattern Other)
	{
		for (int i = 0; i < 9; i++)
		{
			Data[i] = Data[i] | Other.Data[i];
		}
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
struct FTilePatternSet
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	TArray<FTilePattern> Patterns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	TArray<float> Weights;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	TArray<float> PlogP;
};



USTRUCT(BlueprintType)
struct FTileMap
{
    GENERATED_BODY()
    FTileMap()
    {
        Size = FTileMapCoords(3,3);
        Data.Init(0, Size.X * Size.Y);
    	TileSizeUU = 100;
    	Origin = FVector::ZeroVector;
    }

    FTileMap(const FTileMapCoords NewSize)
    {
    	Size = NewSize;
    	Data.Init(0, Size.X * Size.Y);
    	TileSizeUU = 100;
    	Origin = FVector::ZeroVector;
    }

    FTileMap(const FTileMapCoords NewSize, const uint8 InitialValue)
    {
    	Size = NewSize;
    	Data.Init(InitialValue, Size.X * Size.Y);
    	TileSizeUU = 100;
    	Origin = FVector::ZeroVector;
    }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	FTileMapCoords Size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	float TileSizeUU;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	FVector Origin;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap", meta = (Bitmask, BitmaskEnum = "ETileState"))
    TArray<uint8> Data;

    uint8 GetTile(const FTileMapCoords Coords) const
    {
        return Data[inline_CoordsToIndex(Coords, Size)];
    }
	
    void SetTile(const uint8 Tile, const FTileMapCoords Coords)
    {
        Data[inline_CoordsToIndex(Coords, Size)] = Tile;
    }

	FVector CoordsToWorldLocation(const FTileMapCoords Coords) const
	{
	    return FVector(Coords.X, Coords.Y, 0) * TileSizeUU + Origin;
    }

	FTileMapCoords WorldLocationToCoords(const FVector& Location) const
	{
    	FTileMapCoords Result;
    	Result.X = (Location - Origin).X / TileSizeUU;
    	Result.Y = (Location - Origin).Y / TileSizeUU;
    	return Result;
    }

	FTileMap GetTilemapSection(const FTileMapCoords Coords, const FTileMapCoords SectionSize) const
	{
    	FTileMap OutSection = FTileMap(SectionSize);
    	for (int32 j = 0; j < SectionSize.Y; j++)
    	{
    		for (int32 i = 0; i < SectionSize.X; i++)
    		{
    			const uint8 Tile = GetTile(FTileMapCoords(Coords.X + i, Coords.Y + j));
    			OutSection.SetTile(Tile, FTileMapCoords(i, j));
    		}
    	}
    	return OutSection;
    }
    

    FTilePattern ReadPattern(const FTileMapCoords StartCoords) const
    {
        FTilePattern Result = FTilePattern();
        for (int32 Y = 0; Y < 3; Y++)
        {
            for (int32 X = 0; X < 3; X++)
            {
            	const FTileMapCoords Coords = FTileMapCoords(X + StartCoords.X, Y + StartCoords.Y);
                const uint8 Tile = GetTile(Coords);
            	const int32 Index = inline_CoordsToIndex(FTileMapCoords(X, Y), FTileMapCoords(3,3));
                Result.Data[Index] = Tile;
            }
        }
        return Result;
    }

    void WritePattern(const FTilePattern Pattern, const FTileMapCoords Coords)
    {
        for (int32 Y = 0; Y < 3; Y++)
        {
            for (int32 X = 0; X < 3; X++)
            {
            	const int32 PatternIndex = inline_CoordsToIndex(FTileMapCoords(X, Y), FTileMapCoords(3, 3));
                const uint8 Tile = Pattern.Data[PatternIndex];
                SetTile(Tile, FTileMapCoords(X + Coords.X, Y + Coords.Y));
            }
        }
    }

	bool bIsValid() const
	{
    	return Data.Num() == Size.X * Size.Y;
    }
	
	FTilePatternSet GeneratePatternSet() const
	{
	    FTilePatternSet PatternSet;
    	//PatternSet.Patterns.Empty();
    	//PatternSet.Counts.Empty();
    	TArray<FTilePattern> Variants;
    	TArray<int32> Counts;
    	Variants.Reserve(8);
    	Counts.Reserve(8);

    	for (int32 y = 0; y < Size.Y - 3 + 1; y++)
    	{
    		for (int32 x = 0; x < Size.X - 3 + 1; x++)
    		{
    			Variants.Reset();
    			FTilePattern Pattern = ReadPattern(FTileMapCoords(x,y));
    			Variants.AddUnique(Pattern);
    			Pattern.Transpose();
    			Variants.AddUnique(Pattern);
    			Pattern.Reflect();
    			Variants.AddUnique(Pattern);
    			Pattern.Transpose();
    			Variants.AddUnique(Pattern);
    			Pattern.Reflect();
    			Variants.AddUnique(Pattern);
    			Pattern.Transpose();
    			Variants.AddUnique(Pattern);
    			Pattern.Reflect();
    			Variants.AddUnique(Pattern);
    			Pattern.Transpose();
    			Variants.AddUnique(Pattern);
    			for (const auto& Variant : Variants)
    			{
    				if (const int Index = PatternSet.Patterns.Find(Variant); Index > -1)
    				{
    					Counts[Index]++;
    				}
    				else
    				{
    					PatternSet.Patterns.Add(Variant);
    					Counts.Add(1);
    				}
    			}
    		}
    	}

    	float SumCounts = 0;
    	for (const auto& Count : Counts)
    	{
    		SumCounts += Count;
    	}

    	PatternSet.Weights.SetNum(Counts.Num());
    	PatternSet.PlogP.SetNum(Counts.Num());
    	for (int32 i = 0; i < Counts.Num(); i++)
    	{
    		const float Probability = Counts[i] / SumCounts;
    		PatternSet.Weights[i] = Probability;
    		PatternSet.PlogP[i] = Probability * log2(Probability);
    		UE_LOG(LogTemp, Warning, TEXT("Pattern %d has count %d, probability %f, and PlogP %f"), i, Counts[i], Probability, PatternSet.PlogP[i]);
    	}
    	return PatternSet;
    }

	void Fill(const uint8 Tile)
    {
    	for (auto& Element : Data)
    	{
    		Element = Tile;
    	}
    }

	TArray<FTileMapCoords> GetConnectedCoords(const FTileMapCoords Coords) const
	{
    	const uint8 Tile = GetTile(Coords);
    	TSet<FTileMapCoords> ConnectedCoords;
		//ConnectedCoords.Add(Coords);
    	TQueue<FTileMapCoords> NeighborsToCheck;
    	NeighborsToCheck.Enqueue(Coords);
    	//NeighborsToCheck.Enqueue(FTileMapCoords(Coords.X - 1, Coords.Y));
    	//NeighborsToCheck.Enqueue(FTileMapCoords(Coords.X + 1, Coords.Y));
    	//NeighborsToCheck.Enqueue(FTileMapCoords(Coords.X, Coords.Y - 1));
    	//NeighborsToCheck.Enqueue(FTileMapCoords(Coords.X, Coords.Y + 1));
	    while (!NeighborsToCheck.IsEmpty())
	    {
		    FTileMapCoords Neighbor;
		    NeighborsToCheck.Dequeue(Neighbor);
		    if (Neighbor.X >= 0 && Neighbor.X < Size.X && Neighbor.Y >= 0 && Neighbor.Y < Size.Y)
		    {
			    uint8 NeighborTile = GetTile(Neighbor);
			    if (NeighborTile == Tile && !ConnectedCoords.Contains(Neighbor))
			    {
				    ConnectedCoords.Add(Neighbor);
			    	NeighborsToCheck.Enqueue(FTileMapCoords(Neighbor.X - 1, Neighbor.Y));
			    	NeighborsToCheck.Enqueue(FTileMapCoords(Neighbor.X + 1, Neighbor.Y));
			    	NeighborsToCheck.Enqueue(FTileMapCoords(Neighbor.X, Neighbor.Y - 1));
			    	NeighborsToCheck.Enqueue(FTileMapCoords(Neighbor.X, Neighbor.Y + 1));
			    }
		    }
	    }
    	return ConnectedCoords.Array();
    }
	
	void Flood(const uint8 Tile, const FTileMapCoords Coords)
    {
    	const uint8 TileToReplace = GetTile(Coords);
    	const TArray<FTileMapCoords> ConnectedCoords = GetConnectedCoords(Coords);
    	for (const auto Coord : ConnectedCoords)
    	{
    		SetTile(Tile, Coord);
    	}
    }
	
	void SetBorder(const uint8 Tile)
    {
    	const int32 LastX = Size.X - 1;
    	const int32 LastY = Size.Y - 1;
    	for (int32 i = 0; i < Size.X; i++)
    	{
    		SetTile(Tile, FTileMapCoords(i, 0));
    		SetTile(Tile, FTileMapCoords(i, LastY));
    	}
    	for (int32 i = 0; i < Size.Y; i++)
    	{
    		SetTile(Tile, FTileMapCoords(0, i));
    		SetTile(Tile, FTileMapCoords(LastX, i));
    	}
    }
	
	UTexture2D* TileMapToTexture()
    {
    	const int32 PixelCount = Size.X * Size.Y;
    	TArray<FColor> Colors;
    	Colors.Reserve(Data.Num());
    	for (const auto& Tile : Data)
    	{
    		Colors.Add(inline_TileToColor(Tile));
    	}

    	UTexture2D* Texture = UTexture2D::CreateTransient(Size.X, Size.Y, PF_B8G8R8A8, "");
    	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

    	FMemory::Memcpy(TextureData, Colors.GetData(), PixelCount * 4);
    	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
    	Texture->Filter = TF_Nearest;
    	Texture->UpdateResource();

    	return Texture;
    }
};

FORCEINLINE bool operator ==(const FTileMap& A, const FTileMap& B)
{
    return A.Size.X == B.Size.X && A.Data == B.Data;
}



UCLASS()
class SLTILEMAP_API USLTilemapLib : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
	//Tests
    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static void RunTests();
	
    //Coordinates
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static int32 CoordsToIndex(const FTileMapCoords Coords, const FTileMapCoords Size);
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static FTileMapCoords IndexToCoords(const int32 Index, const FTileMapCoords Size);
	
    //TileMap
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static FTileMap CreateTileMap(const FTileMapCoords Size, const uint8 InitialValue);
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static bool IsTilemapValid(const FTileMap& TileMap);
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static uint8 GetTileAtCoords(const FTileMap& TileMap, const FTileMapCoords Coords);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static void SetTileAtCoords(UPARAM(ref) FTileMap& TileMap, const uint8 Tile, const FTileMapCoords Coords);
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static FVector CoordsToWorldLocation(const FTileMap& TileMap, const FTileMapCoords Coords);
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static FTileMapCoords WorldLocationToCoords(const FTileMap& TileMap, const FVector& Location);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static void Fill(UPARAM(ref) FTileMap& TileMap, const uint8 Tile);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static void Flood(UPARAM(ref) FTileMap& TileMap, const uint8 Tile, const FTileMapCoords Coords);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static void SetBorder(UPARAM(ref) FTileMap& TileMap, const uint8 Tile);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static FTilePatternSet GeneratePatternSet(UPARAM(ref) FTileMap& TileMap);
	
	//Visualization
    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static UTexture2D* TileMapToTexture(UPARAM(ref) FTileMap& TileMap);
    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static FColor TileToColor(const uint8 Tile);
};
