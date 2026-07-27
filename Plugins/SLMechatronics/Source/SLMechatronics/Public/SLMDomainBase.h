// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMSystemBase.h"
#include "SLMTypes.h"


class SLMECHATRONICS_API FSLMDomainSystemBase : public FSLMSystemBase
{
public:
	
	virtual TUniquePtr<FSLMDomainSystemBase> CreateInstance() const = 0;
	
	//Connections
	bool DoesConnectionExist(const FSLMConnection& Connection);
	void AddConnection(const FSLMConnection& Connection);
	void RemoveConnection(const FSLMConnection& Connection);
	
	

	//Spatial Queries
	bool WorldLocationToPortAddress(const FSLMSpatialContextRuntime& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress);
	bool PortAddressToWorldLocation(const FSLMPortAddress& PortAddress, FVector& OutWorldLocation);
	void AppendPortAddressesForDevices(const TArray<FSLMDeviceAddress>& DeviceAddresses, TArray<FSLMPortAddress>& PortAddresses) const;
	void AppendConnectionsForPortAddresses(const TArray<FSLMPortAddress>& PortAddresses, TArray<FSLMConnection>& Connections) const;

	//Debug
	virtual void DebugDraw() override;
	virtual FString GetPortDebugString(const FSLMPortAddress& Address);
	virtual void RunTests();

protected:
	TSet<int32> GetConnectedPortIDs(const TSet<int32>& Roots) const;
	bool ArePortsConnected(const int32 FirstPortID, const int32 SecondPortID);

private:
	virtual void CleanUp() override;
	void CleanContainers();
	void ProcessPortsToRemove();
	void ProcessConnectionsPending();
	void ProcessConnectionsToAdd();
	void ProcessConnectionsToRemove();
	void UpdateAdjacencyList();
	void GatherDirtyPortIDs();
	virtual void DissolveAndRebuildParticles() = 0;
	virtual void DestroyPorts() = 0;
	FVector PortIDToWorldLocation(const int32 PortID);
	int32 WorldLocationToPortID(const FSLMSpatialContextRuntime& Filter, const FVector& WorldLocation);

protected:
	TSet<FSLMConnection> ConnectionsToAdd;
	TSet<FSLMConnection> ConnectionsPending;
	TSet<FSLMConnection> ConnectionsToRemove;
	TSet<FSLMPortAddress> PortsToRemove;
	
	TArray<TTuple<int32, int32>> ConnectionsToAddByID;
	TArray<TTuple<int32, int32>> ConnectionsToRemoveByID;

	TMap<FSLMPortAddress, int32> PortAddressToPortID;
	TSparseArray<FSLMPortAddress> PortIDToPortAddress;
	TSparseArray<int32> PortIDToParticleID;
	TSparseArray<FSLMSpatialContextAuthored> PortSpatialContextAuthored;
	TSparseArray<FSLMSpatialContextRuntime> PortSpatialContextRuntime;
	
	TMultiMap<int32, int32> AdjacencyList;
	TSet<int32> DirtyPortIDs;
	
	
	TMultiMap<FSLMDeviceAddress, FSLMPortAddress> DeviceAddressToPortAddresses;
	
	TMultiMap<AActor*, int32> ActorToPortIDs;
	TMap<int32, AActor*> PortIDToActor;
};

