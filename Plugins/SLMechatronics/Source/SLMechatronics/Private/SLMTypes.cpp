// Copyright Spectrelight Studios, LLC. All Rights Reserved.


#include "SLMTypes.h"

#include "SLMDeviceBase.h"
#include "SLMDomainBase.h"


FString FSLMDeviceAddress::GetDebugString() const
{
	FString Result;
	Result += FString::Format(TEXT("{0}, {1}"), {DeviceClass->GetName(),DeviceID});
	return Result;
}

bool FSLMDeviceAddress::IsValid() const
{
	return DeviceClass && DeviceID >= 0;
}

uint32 GetTypeHash(const struct FSLMDeviceAddress& Address)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Address.DeviceClass));
	Hash = HashCombine(Hash, GetTypeHash(Address.DeviceID));
	return Hash;
}

bool operator==(const struct FSLMDeviceAddress& A, const struct FSLMDeviceAddress& B)
{
	return A.DeviceClass == B.DeviceClass && A.DeviceID == B.DeviceID;
}

FSLMDeviceAddress MakeDeviceAddress(const USLMDeviceSubsystemBase* Device, const int32 DeviceID)
{
	return {Device->GetClass(), DeviceID};
}

FString FSLMPortAddress::GetDebugString() const
{
	FString Result;
	Result += FString::Format(TEXT("{0}, {1}, {2}, {3}"), {DeviceAddress.DeviceClass->GetName(),DeviceAddress.DeviceID,DomainClass->GetName(),PortID});
	return Result;
}

bool FSLMPortAddress::IsValid() const
{
	return DeviceAddress.IsValid() && DomainClass && PortID >= 0;
}

uint32 GetTypeHash(const FSLMPortAddress& Address)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Address.DeviceAddress));
	Hash = HashCombine(Hash, GetTypeHash(Address.DomainClass));
	Hash = HashCombine(Hash, GetTypeHash(Address.PortID));
	return Hash;
}

bool operator==(const FSLMPortAddress& A, const FSLMPortAddress& B)
{
	return A.DeviceAddress == B.DeviceAddress && A.DomainClass == B.DomainClass && A.PortID == B.PortID;
}

FSLMPortAddress MakePortAddress(const USLMDeviceSubsystemBase* Device, const USLMDomainSubsystemBase* Domain, const int32 DeviceID, const int32 PortID)
{
	const FSLMDeviceAddress DeviceAddress = MakeDeviceAddress(Device, DeviceID);
	return {DeviceAddress, Domain->GetClass(), PortID};
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
	const bool bDomainClassesMatch = First.DomainClass == Second.DomainClass;
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
