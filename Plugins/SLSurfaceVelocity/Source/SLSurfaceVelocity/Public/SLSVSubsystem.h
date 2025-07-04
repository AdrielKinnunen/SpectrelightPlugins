// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "Chaos/MidPhaseModification.h"
#include "Subsystems/WorldSubsystem.h"
#include "Components/PrimitiveComponent.h"

#include "Chaos/SimCallbackObject.h"
#include "Chaos/SimCallbackInput.h"
#include "Chaos/ParticleHandle.h"

#include "SLSVSubsystem.generated.h"


/*
USTRUCT()
struct FChaosParticlePair
{
	GENERATED_BODY()

	Chaos::TPBDRigidParticleHandle<Chaos::FReal, 3>* ParticleHandle0;
	Chaos::TPBDRigidParticleHandle<Chaos::FReal, 3>* ParticleHandle1;

	FChaosParticlePair()
	{
		ParticleHandle0 = nullptr;
		ParticleHandle1 = nullptr;
	}

	FChaosParticlePair(Chaos::TPBDRigidParticleHandle<Chaos::FReal, 3>* pH1, Chaos::TPBDRigidParticleHandle<Chaos::FReal, 3>* pH2)
	{
		ParticleHandle0 = pH1;
		ParticleHandle1 = pH2;
	}

	FORCEINLINE bool operator==(const FChaosParticlePair& Other) const
	{
		return(
			(ParticleHandle0 == Other.ParticleHandle0 || ParticleHandle0 == Other.ParticleHandle1) &&
			(ParticleHandle1 == Other.ParticleHandle1 || ParticleHandle1 == Other.ParticleHandle0)
			);
	}
};
*/



struct FTestSimCallbackInput : public Chaos::FSimCallbackInput
{
	void Reset() {}
};

struct FTestSimCallbackNoOutput : public Chaos::FSimCallbackOutput
{
	void Reset() {}
};

class FTestCallback : public Chaos::TSimCallbackObject<FTestSimCallbackInput, FTestSimCallbackNoOutput, Chaos::ESimCallbackOptions::ContactModification | Chaos::ESimCallbackOptions::Presimulate>
{

private:
	
	Chaos::FReal DeltaTime = 0;
	
	virtual void OnPreSimulate_Internal() override;
	virtual void OnContactModification_Internal(Chaos::FCollisionContactModifier& Modifier) override;
	virtual void OnPreSolve_Internal() override;
	virtual void OnPostSolve_Internal() override;
};

USTRUCT()
struct FCollisionPrimPair
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> Prim1;
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> Prim2;

	FCollisionPrimPair()
	{
		Prim1 = nullptr;
		Prim2 = nullptr;
	}

	FORCEINLINE bool operator==(const FCollisionPrimPair& Other) const
	{
		if (!IsValid(Prim1) || !IsValid(Prim2))
			return false;

		if (!IsValid(Other.Prim1) || !IsValid(Other.Prim2))
			return false;
		
		return(
			(Prim1.Get() == Other.Prim1.Get() || Prim1.Get() == Other.Prim2.Get()) &&
			(Prim2.Get() == Other.Prim1.Get() || Prim2.Get() == Other.Prim2.Get())
			);
	}

	friend uint32 GetTypeHash(const FCollisionPrimPair& InKey)
	{
		return GetTypeHash(InKey.Prim1) ^ GetTypeHash(InKey.Prim2);
	}

};

UCLASS()
class SLSURFACEVELOCITY_API UTestFrictionModificationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:


	UTestFrictionModificationSubsystem() :
	Super()
	{
		ContactModifierCallback = nullptr;
	}

	FTestCallback* ContactModifierCallback;
	
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
		// Not allowing for editor type as this is a replication subsystem
	}

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
private:
};