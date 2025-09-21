// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapColor.generated.h"



UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ETileState : uint8
{
	None = 0 UMETA(Hidden),
	Void = 1,
	Ground = 2,
	Wall = 4,
	Window = 8,
	RoofedVoid = 16,
	RoofedGround = 32,
	RoofedWall = 64,
	RoofedWindow = 128
};
ENUM_CLASS_FLAGS(ETileState);

namespace SLTileMap
{
	inline FColor TileToColor(const uint8 Tile)
	{
		const ETileState Flags = static_cast<ETileState>(Tile);
		const uint8 Void = EnumHasAnyFlags(Flags, ETileState::Void | ETileState::RoofedVoid);
		const uint8 Ground = EnumHasAnyFlags(Flags, ETileState::Ground | ETileState::RoofedGround);
		const uint8 Wall = EnumHasAnyFlags(Flags, ETileState::Wall | ETileState::RoofedWall);
		const uint8 Window = EnumHasAnyFlags(Flags, ETileState::Window | ETileState::RoofedWindow);
		const uint8 Roofed = EnumHasAnyFlags(Flags, ETileState::RoofedVoid | ETileState::RoofedGround | ETileState::RoofedWall | ETileState::RoofedWindow);
		const uint8 IsPow2 = (Tile & (Tile - 1)) == 0;
	
		uint8 R = (6 * IsPow2 + 2) * (Wall * 16 + Window * 16);
		uint8 G = (6 * IsPow2 + 2) * (Window * 16 + Ground * 16);
		uint8 B = (6 * IsPow2 + 2) * (Void * 16 + Ground * 8 + Window * 8);
		const uint8 A = 255 * Roofed;
		//R *= IsPow2;
		//G *= IsPow2;
		//B *= IsPow2;
    
		return FColor(R, G, B, A);
	}
	
	inline UTexture2D* TileMapToTexture(const FTileMap& TileMap)
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
}

