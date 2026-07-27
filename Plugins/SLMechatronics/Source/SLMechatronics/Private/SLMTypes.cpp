// Copyright Spectrelight Studios, LLC. All Rights Reserved.

#include "SLMTypes.h"

#include "SLMDeviceBase.h"
#include "SLMDomainBase.h"


FString FSLMDeviceAddress::GetDebugString() const
{
	FString Result;
	Result += FString::Format(TEXT("{0}, {1}"), {DeviceTag.ToString(),DeviceID});
	return Result;
}

bool FSLMDeviceAddress::IsValid() const
{
	return DeviceTag.IsValid() && DeviceID >= 0;
}

uint32 GetTypeHash(const struct FSLMDeviceAddress& Address)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Address.DeviceTag));
	Hash = HashCombine(Hash, GetTypeHash(Address.DeviceID));
	return Hash;
}

bool operator==(const struct FSLMDeviceAddress& A, const struct FSLMDeviceAddress& B)
{
	return A.DeviceTag == B.DeviceTag && A.DeviceID == B.DeviceID;
}

FSLMDeviceAddress MakeDeviceAddress(const FSLMDeviceSystemBase* DeviceSystem, const int32 DeviceID)
{
	return {DeviceSystem->SystemTag, DeviceID};
}








FString FSLMPortAddress::GetDebugString() const
{
	FString Result;
	Result += FString::Format(TEXT("{0}, {1}, {2}, {3}"), {DeviceAddress.DeviceTag.ToString(),DeviceAddress.DeviceID,DomainTag.ToString(),PortID});
	return Result;
}

bool FSLMPortAddress::IsValid() const
{
	return DeviceAddress.IsValid() && DomainTag.IsValid() && PortID >= 0;
}

uint32 GetTypeHash(const FSLMPortAddress& Address)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Address.DeviceAddress));
	Hash = HashCombine(Hash, GetTypeHash(Address.DomainTag));
	Hash = HashCombine(Hash, GetTypeHash(Address.PortID));
	return Hash;
}

bool operator==(const FSLMPortAddress& A, const FSLMPortAddress& B)
{
	return A.DeviceAddress == B.DeviceAddress && A.DomainTag == B.DomainTag && A.PortID == B.PortID;
}

FSLMPortAddress MakePortAddress(const FSLMDeviceSystemBase* DeviceSystem, const int32 DeviceID, const FSLMDomainSystemBase* DomainSystem, const int32 PortID)
{
	return {MakeDeviceAddress(DeviceSystem, DeviceID), DomainSystem->SystemTag, PortID};
}









FString FSLMConnection::GetDebugString() const
{
	FString Result;
	Result += FString::Format(TEXT("\nPort {0} is connected to Port {1}"), {First.GetDebugString(), Second.GetDebugString()});
	return Result;
}

bool FSLMConnection::IsValid() const
{
	const bool bAddressesValid = First.IsValid() && Second.IsValid();
	const bool bDomainClassesMatch = First.DomainTag == Second.DomainTag;
	const bool bNotSelfConnection = First != Second;
	return bAddressesValid && bDomainClassesMatch && bNotSelfConnection;	
}

uint32 GetTypeHash(const FSLMConnection& Connection)
{
	return GetTypeHash(Connection.First) ^ GetTypeHash(Connection.Second);
}

bool operator==(const FSLMConnection& A, const FSLMConnection& B)
{
	const bool AA = A.First == B.First && A.Second == B.Second;
	const bool AB = A.Second == B.First && A.First == B.Second;
	return AA || AB;
}

FString FSLMSpatialContextAuthored::GetDebugString() const
{
	FString Result;
	Result += FString::Format(TEXT("\n{0}, {1}, {2}"), {SceneComponentName.ToString(),SocketName.ToString(),OffsetLocal.ToString()});
	return Result;
}

uint32 FSLMSpatialContextAuthored::GetDebugHash(const bool bVerbose) const
{
	uint32 Result = 0;
	Result = HashCombine(Result, GetTypeHash(SceneComponentName));
	Result = HashCombine(Result, GetTypeHash(SocketName));
	Result = HashCombine(Result, GetTypeHash(OffsetLocal));
	return Result;
}

FTransform FSLMSpatialContextRuntime::GetWorldTransform() const
{
	FTransform Result;
	if (const auto Scene = AssociatedSceneComponent.Get())
	{
		Result = Scene->GetSocketTransform(SocketName);
		Result.SetLocation(Result.TransformPosition(OffsetLocal));
		return Result;
	}
	if (const auto Actor = AssociatedActor.Get())
	{
		Result = Actor->GetTransform();
		Result.SetLocation(Result.TransformPosition(OffsetLocal));
		return Result;
	}
	Result.SetLocation(Result.TransformPosition(OffsetLocal));
	return Result;
}

FSLMSpatialContextRuntime FSLMSpatialContextRuntime::MakeFromContextAuthored(const FSLMSpatialContextAuthored& ContextAuthored, AActor* AssociatedActor)
{
	FSLMSpatialContextRuntime Result;
	Result.AssociatedActor = AssociatedActor;
	USceneComponent* SceneComp = nullptr;
	if (AssociatedActor)
	{
		for (UActorComponent* Component : AssociatedActor->GetComponents())
		{
			if (Component->GetFName() == ContextAuthored.SceneComponentName)
			{
				SceneComp = Cast<USceneComponent>(Component);
				break;
			}
		}
	}
	if (SceneComp)
	{
		Result.AssociatedSceneComponent = SceneComp;
	}
	Result.SocketName = ContextAuthored.SocketName;
	Result.OffsetLocal = ContextAuthored.OffsetLocal;
	return Result;
}
