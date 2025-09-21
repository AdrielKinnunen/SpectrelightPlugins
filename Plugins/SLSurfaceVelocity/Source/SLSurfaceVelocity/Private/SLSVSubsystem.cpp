// Copyright Spectrelight Studios, LLC

#include "SLSVSubsystem.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Chaos/KinematicGeometryParticles.h"
#include "PBDRigidsSolver.h"
#include "Chaos/ContactModification.h"
//#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
//#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
//#include "Chaos/ParticleHandle.h"
//#include "PhysicsEngine/PhysicsAsset.h"
//#include "PhysicsEngine/SkeletalBodySetup.h"


void FTestCallback::OnPreSimulate_Internal()
{
	DeltaTime = GetDeltaTime_Internal();
}

void FTestCallback::OnContactModification_Internal(Chaos::FCollisionContactModifier& Modifier)
{
	const FTestSimCallbackInput* Input = GetConsumerInput_Internal();

	for (Chaos::FContactPairModifierIterator ContactIterator = Modifier.Begin(); ContactIterator; ++ContactIterator)
	{
		if (ContactIterator.IsValid())
		{
			Chaos::TVec2<Chaos::FGeometryParticleHandle*> Pair = ContactIterator->GetParticlePair();

			Chaos::TPBDRigidParticleHandle<Chaos::FReal, 3>* ParticleHandle0 = Pair[0]->CastToRigidParticle();
			Chaos::TPBDRigidParticleHandle<Chaos::FReal, 3>* ParticleHandle1 = Pair[1]->CastToRigidParticle();;

			
			//Chaos::FReal DeltaTime = GetDeltaTime_Internal();
			
			
			//ContactIterator->ModifyParticlePosition(ModifiedPos, true, 0);
			Chaos::FVec3 Loc0 = Chaos::FVec3(0,0,0);
			Chaos::FVec3 Loc1 = Chaos::FVec3(0,0,0);
			ContactIterator->GetWorldContactLocations(0, Loc0, Loc1);
			Chaos::FVec3 Loc1Modified = Loc1 + Chaos::FVec3(10,0,0) * DeltaTime;
			ContactIterator->ModifyWorldContactLocations(Loc0, Loc1Modified, 0);
			
			if (ParticleHandle0)
			{
				//ParticleHandle0->SetLinearImpulseVelocity(Chaos::TVector<double, 3>(213, 0, 0));
				//ParticleHandle0->SetV(Chaos::TVector<double, 3>(213, 0, 0));
			}
			if (ParticleHandle1)
			{
				//ParticleHandle1->SetLinearImpulseVelocity(Chaos::TVector<double, 3>(213, 0, 0));
				//ParticleHandle1->SetV(Chaos::TVector<double, 3>(213, 0, 0));
			}
		}
	}
}

void FTestCallback::OnPreSolve_Internal()
{
	DeltaTime = GetDeltaTime_Internal();
}

void FTestCallback::OnPostSolve_Internal()
{
	//this->GetProducerInputData_External();
}


void UTestFrictionModificationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//auto* Solver = GetWorld()->GetPhysicsScene()->GetSolver();
	//ContactModifierCallback = Solver->CreateAndRegisterSimCallbackObject_External<FTestCallback>();
}

void UTestFrictionModificationSubsystem::Deinitialize()
{
	auto* Solver = GetWorld()->GetPhysicsScene()->GetSolver();
	if (Solver && ContactModifierCallback)
	{
		Solver->UnregisterAndFreeSimCallbackObject_External(ContactModifierCallback);
		ContactModifierCallback = nullptr;		
	}
	Super::Deinitialize();
}

void UTestFrictionModificationSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	auto* Solver = GetWorld()->GetPhysicsScene()->GetSolver();
	ContactModifierCallback = Solver->CreateAndRegisterSimCallbackObject_External<FTestCallback>();
}


