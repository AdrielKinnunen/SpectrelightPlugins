// Fill out your copyright notice in the Description page of Project Settings.


#include "SLTilemapSubsystem.h"


void USLTilemapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Wave = NewObject<USLWave>();
}

void USLTilemapSubsystem::Deinitialize()
{
}
