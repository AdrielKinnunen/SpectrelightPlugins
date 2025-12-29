// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapColor.generated.h"



UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ETileMapColor : uint8
{
	None       = 0 UMETA(Hidden), // A 'None' or 'Default' value is good practice
	Blue       = 1<<0,
	Green      = 1<<1,
	Red        = 1<<2,
	Yellow     = 1<<3,
	DarkBlue   = 1<<4,
	DarkGreen  = 1<<5,
	DarkRed    = 1<<6,
	DarkYellow = 1<<7,
};

ENUM_CLASS_FLAGS(ETileMapColor)

namespace SLTileMap
{
	inline FColor TileToColor(const uint8 Tile)
	{
		const ETileMapColor Flags = static_cast<ETileMapColor>(Tile);
		const uint8 Blue = EnumHasAnyFlags(Flags, ETileMapColor::Blue | ETileMapColor::DarkBlue);
		const uint8 Green = EnumHasAnyFlags(Flags, ETileMapColor::Green | ETileMapColor::DarkGreen);
		const uint8 Red = EnumHasAnyFlags(Flags, ETileMapColor::Red | ETileMapColor::DarkRed);
		const uint8 Yellow = EnumHasAnyFlags(Flags, ETileMapColor::Yellow | ETileMapColor::DarkYellow);
		const uint8 Dark = EnumHasAnyFlags(Flags, ETileMapColor::DarkBlue | ETileMapColor::DarkGreen | ETileMapColor::DarkRed | ETileMapColor::DarkYellow);
		const uint8 IsPow2 = (Tile & (Tile - 1)) == 0;
	
		uint8 R = (6 * IsPow2 + 2) * (Red * 16 + Yellow * 16);
		uint8 G = (6 * IsPow2 + 2) * (Yellow * 16 + Green * 16);
		uint8 B = (6 * IsPow2 + 2) * (Blue * 16 + Green * 8 + Yellow * 8);
		const uint8 A = 255 * Dark;
		//R *= IsPow2;
		//G *= IsPow2;
		//B *= IsPow2;
    
		return FColor(R, G, B, A);
	}
	
	inline UTexture2D* TileMap8ToTexture(const FTileMap& TileMap)
	{
		const int32 PixelCount = TileMap.Size.X * TileMap.Size.Y;
		TArray<FColor> Colors;
		Colors.Reserve(TileMap.Data.Num());
		for (const auto& Tile : TileMap.Data)
		{
			Colors.Add(TileToColor(Tile));
		}

		UTexture2D* Texture = UTexture2D::CreateTransient(TileMap.Size.X, TileMap.Size.Y, PF_B8G8R8A8, "");
		void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

		FMemory::Memcpy(TextureData, Colors.GetData(), PixelCount * 4);
		Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
		Texture->Filter = TF_Nearest;
		Texture->UpdateResource();

		return Texture;
	}


	inline FColor DistanceToColor(const uint8 Distance, const uint8 MaxDistance)
	{
		const float Alpha = static_cast<float>(Distance) / static_cast<float>(MaxDistance);
		FLinearColor Color = {Alpha * 360, 1, 1, 0};
		Color = Color.HSVToLinearRGB();
		if (Distance > 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Distance was %i, MaxDistance was %i, Alpha was %f"), Distance, MaxDistance, Alpha);
			//UE_LOG(LogTemp, Warning, TEXT("Color ended up being %f, %f, %f, %f"), Color.R, Color.G, Color.B, Color.A);
		}
		return Color.ToFColor(true);
	}


	

	inline UTexture2D* DistanceFieldToTexture(const FTileMap& TileMap)
	{
		const int32 PixelCount = TileMap.Size.X * TileMap.Size.Y;
		//UE_LOG(LogTemp, Warning, TEXT("PixelCount is %i"), PixelCount);
		TArray<FColor> Colors;
		Colors.Reserve(TileMap.Data.Num());
		uint8 MaxDistance = 0;
		for (const auto& Distance : TileMap.Data)
		{
			MaxDistance = FMath::Max(MaxDistance, Distance);
		}
		for (const auto& Distance : TileMap.Data)
		{
			const FColor Color = DistanceToColor(Distance, MaxDistance);
			if (Distance > 0)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Color ended up being %i, %i, %i, %i"), Color.R, Color.G, Color.B, Color.A);
			}
			Colors.Add(Color);
		}

		//UE_LOG(LogTemp, Warning, TEXT("Color array is size %i"), Colors.Num());


		
		UTexture2D* Texture = UTexture2D::CreateTransient(TileMap.Size.X, TileMap.Size.Y, PF_B8G8R8A8, "");
		void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

		FMemory::Memcpy(TextureData, Colors.GetData(), PixelCount * 4);
		Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
		Texture->Filter = TF_Nearest;
		Texture->UpdateResource();

		return Texture;
	}
}

