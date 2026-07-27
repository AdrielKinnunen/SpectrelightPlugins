// Copyright Spectrelight Studios, LLC

#pragma once
#include "SLMDomainBase.h"


template<typename SystemType, typename DataType, typename PortType>
class TSLMDomainSystem : public FSLMDomainSystemBase
{
public:
	
	virtual TUniquePtr<FSLMDomainSystemBase> CreateInstance() const override
	{
		return MakeUnique<SystemType>();
	}
	
	DataType& GetParticleRef(const int32 PortID)
	{
		return Particles[PortIDToParticleID[PortID]];
	}
	
	int32 RegisterPort(const PortType& Port, const FSLMPortAddress& PortAddress, AActor* AssociatedActor)
	{
		const bool bPortExists = PortAddressToPortID.Contains(PortAddress);
		if (bPortExists)
		{
			return EditPort(Port, PortAddress, AssociatedActor);
		}
		return AddPort(Port, PortAddress, AssociatedActor);
	}

	void RemovePort(const FSLMPortAddress& PortAddress)
	{
		PortsToRemove.Add(PortAddress);
		bNeedsCleanUp = true;
	}

	virtual uint32 GetDebugHash(const bool bVerbose) override
	{
		uint32 Result = 0;
		Result = HashCombine(Result, GetTypeHash(PortDefaults.Num()));
		Result = HashCombine(Result, GetTypeHash(PortIDToParticleID.Num()));
		Result = HashCombine(Result, GetTypeHash(Particles.Num()));
		for (const auto Entry : PortAddressToPortID)
		{
			const FSLMPortAddress PortAddress = Entry.Key;
			//const DataType& PortDefault = PortDefaults[Entry.Value];
			const DataType& Particle = Particles[PortIDToParticleID[Entry.Value]];
			const FSLMSpatialContextAuthored& SpatialContext = PortSpatialContextAuthored[Entry.Value];
			uint32 EntryHash = 0;
			EntryHash = HashCombine(EntryHash, GetTypeHash(PortAddress));
			//EntryHash = HashCombine(EntryHash, PortDefault.GetDebugHash(bVerbose));
			EntryHash = HashCombine(EntryHash, Particle.GetDebugHash(bVerbose));
			EntryHash = HashCombine(EntryHash, SpatialContext.GetDebugHash(bVerbose));
			
			Result = Result ^ EntryHash;
		}
		return Result;
	}

	virtual FString GetDebugString(const bool bVerbose) override
	{
		FString Result;
		Result += FString::Format(TEXT("\n---------- {0} -----------"), {SystemTag.ToString()});
		Result += FString::Format(TEXT("\nHas {0} PortDefaults"), {PortDefaults.Num()});
		Result += FString::Format(TEXT("\nHas {0} PortIDToParticleID"), {PortIDToParticleID.Num()});
		Result += FString::Format(TEXT("\nHas {0} Particles"), {Particles.Num()});
		if (bVerbose)
		{
			for (int32 PortID = 0; PortID < PortIDToParticleID.GetMaxIndex(); PortID++)
			{
				if (PortIDToParticleID.IsValidIndex(PortID))
				{
					Result += FString::Format(TEXT("\nPort {0} maps to Particle {1} with state {2}"), {PortID, PortIDToParticleID[PortID], Particles[PortIDToParticleID[PortID]].GetDebugString()});
					Result += FString::Format(TEXT("\nPort {0} metadata =  {1}"), {PortID, PortSpatialContextAuthored[PortID].GetDebugString()});					
				}
			}
		}
		return Result;
	}

	virtual FString GetPortDebugString(const FSLMPortAddress& Address) override
	{
		FString Result;
		if (const int32* PortIDPtr = PortAddressToPortID.Find(Address))
		{
			const int32 PortID = *PortIDPtr;
			if (PortIDToParticleID.IsValidIndex(PortID))
			{
				const int32 ParticleID = PortIDToParticleID[PortID];
				const DataType& PortDefault = PortDefaults[PortID];
				const DataType& Particle = Particles[ParticleID];
				Result += FString::Format(TEXT("\nPort {0} maps to Particle {1}"), {PortID, ParticleID});
				Result += FString::Format(TEXT("\nPort Default: {0}"), {PortDefault.GetDebugString()});
				Result += FString::Format(TEXT("\nParticle: {0}"), {Particle.GetDebugString()});			
			}
		}
		return Result;
	}

	
	
	
private:
	SystemType* GetSystem()
	{
		return static_cast<SystemType*>(this);
	}
	
	
	int32 AddPort(const PortType& Port, const FSLMPortAddress& PortAddress, AActor* AssociatedActor)
	{
		//IDs, mappings, and Port Data
		const int32 PortID = PortDefaults.Add(Port.PortData);
		const int32 ParticleID = Particles.Add(Port.PortData);
		PortIDToParticleID.EmplaceAt(PortID, ParticleID);
		PortAddressToPortID.Add(PortAddress, PortID);
		PortIDToPortAddress.EmplaceAt(PortID, PortAddress);
		
		//Spatial Context
		PortSpatialContextRuntime.EmplaceAt(PortID, FSLMSpatialContextRuntime::MakeFromContextAuthored(Port.SpatialContext, AssociatedActor));
		PortSpatialContextAuthored.EmplaceAt(PortID, Port.SpatialContext);

		ActorToPortIDs.Add(AssociatedActor, PortID);
		PortIDToActor.Add(PortID, AssociatedActor);
		DeviceAddressToPortAddresses.Add(PortAddress.DeviceAddress, PortAddress);
		
		return PortID;
	}
	
	
	
	int32 EditPort(const PortType& Port, const FSLMPortAddress& PortAddress, AActor* AssociatedActor)
	{
		const int32 PortID = PortAddressToPortID.FindChecked(PortAddress);
		PortDefaults.EmplaceAt(PortID, Port.PortData);
		
		PortSpatialContextRuntime.EmplaceAt(PortID, FSLMSpatialContextRuntime::MakeFromContextAuthored(Port.SpatialContext, AssociatedActor));
		PortSpatialContextAuthored.EmplaceAt(PortID, Port.SpatialContext);
		
		AActor** OldActorPtr = PortIDToActor.Find(PortID);
		AActor* OldActor = OldActorPtr ? *OldActorPtr : nullptr;
		ActorToPortIDs.Remove(OldActor, PortID);
		PortIDToActor.Remove(PortID);
		
		ActorToPortIDs.Add(AssociatedActor, PortID);
		PortIDToActor.Add(PortID, AssociatedActor);
		
		return PortID;
	}
	
	
	
	

	virtual void DissolveAndRebuildParticles() override
	{
		//Dissolve all Particle values back into port data
		TSet<int32> ParticleIDsToRemove;
		for (const auto& PortID : DirtyPortIDs)
		{
			const int32 ParticleID = PortIDToParticleID[PortID];
			DataType::Dissolve(Particles[ParticleID], PortDefaults[PortID]);
			//GetSystem()->Dissolve(Particles[ParticleID], PortDefaults[PortID]);
			ParticleIDsToRemove.Add(ParticleID);
		}

		//Remove all dirty port's Particles
		for (const auto& ParticleID : ParticleIDsToRemove)
		{
			Particles.RemoveAt(ParticleID);
		}

		//Create a Particle for each set of connected dirty ports
		TSet<int32> VisitedIDs;
		for (const auto& PortID : DirtyPortIDs)
		{
			if (!VisitedIDs.Contains(PortID))
			{
				auto ConnectedIDs = GetConnectedPortIDs({PortID});
				VisitedIDs.Append(ConnectedIDs);
				CreateParticleForPorts(ConnectedIDs.Array());
			}
		}
	}

	virtual void DestroyPorts() override
	{
		for (const auto& Address : PortsToRemove)
		{
			RemovePortAtAddress(Address);
		}
		PortsToRemove.Empty(16);
	}

	void CreateParticleForPorts(const TArray<int32>& PortIDs)
	{
		const int32 ParticleID = Particles.Add(DataType());
    
		TArray<DataType> Sources;
		for (const auto& PortID : PortIDs)
		{
			Sources.Add(PortDefaults[PortID]);
			PortIDToParticleID[PortID] = ParticleID;
		}
    
		DataType::Merge(Sources, Particles[ParticleID]);
	}

	void RemovePortAtAddress(const FSLMPortAddress& PortAddress)
	{
		const int32 PortID = PortAddressToPortID.FindChecked(PortAddress);
		Particles.RemoveAt(PortIDToParticleID[PortID]);
		PortIDToParticleID.RemoveAt(PortID);
		PortDefaults.RemoveAt(PortID);
		PortAddressToPortID.Remove(PortAddress);
		PortSpatialContextAuthored.RemoveAt(PortID);
		PortSpatialContextRuntime.RemoveAt(PortID);
		PortIDToPortAddress.RemoveAt(PortID);
		AActor** ActorPtr = PortIDToActor.Find(PortID);
		AActor* Actor = ActorPtr ? *ActorPtr : nullptr;
		ActorToPortIDs.Remove(Actor, PortID);
		PortIDToActor.Remove(PortID);
		
		DeviceAddressToPortAddresses.Remove(PortAddress.DeviceAddress, PortAddress);
	}
	
	

protected:
	TSparseArray<DataType> PortDefaults;
	TSparseArray<DataType> Particles;
};









