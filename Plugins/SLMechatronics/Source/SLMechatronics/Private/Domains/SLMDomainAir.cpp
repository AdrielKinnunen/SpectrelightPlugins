// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainAir.h"

//constexpr float GammaAir = 1.4;				//Specific heat ratio for air
//constexpr float IdealGasConstant = 0.0821;	//Ideal gas constant for atm*L/(mol*K)
//constexpr float MolarMassAir = 28.97;		//Molar mass of air in g/mol


int32 USLMDomainAir::AddPort(const FSLMPortAir& Port)
{
	const int32 PortIndex = Ports.Add(Port);
	PortsRecentlyAdded.Add(PortIndex);
	PortIndexToNetworkIndex.Add(-1);
	bNeedsCleanup = true;
	return PortIndex;
}

void USLMDomainAir::RemovePort(const int32 PortIndex)
{
	PortsToRemove.Add(PortIndex);
	bNeedsCleanup = true;
}

FSLMDataAir USLMDomainAir::GetCopy(const int32 PortIndex)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(NetworkIndex >= 0);
	return Networks[NetworkIndex];
}

FSLMDataAir& USLMDomainAir::GetRef(const int32 PortIndex)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(NetworkIndex >= 0);
	return Networks[NetworkIndex];
}

FSLMDataAir USLMDomainAir::RemoveAir(const int32 PortIndex, const float VolumeLiters)
{
	return FSLMDataAir();
}

void USLMDomainAir::AddAir(const int32 PortIndex, const FSLMDataAir AirToAdd)
{
}

/*
void USLMDomainAir::WriteData(const int32 PortIndex, const float Data)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(NetworkIndex >= 0);
	Networks[NetworkIndex].Write = Data;
}

void USLMDomainAir::PostSimulate(const float DeltaTime)
{
	for (auto& Network : Networks)
	{
	}
}
*/

void USLMDomainAir::CreateNetworkForPorts(const TArray<int32> PortIndices)
{
	const int32 NetworkIndex = Networks.Add(FSLMDataAir());
	float SumVolume = 0.0;
	float SumTemp = 0.0;
	//float Oxygen = kgPerLiterAtSSL;
	for (const auto& PortIndex : PortIndices)
	{
		const auto Data = Ports[PortIndex].DefaultData;
		
		SumVolume += Data.Volume_l;
		SumTemp += Data.Temp_K;
		
		PortIndexToNetworkIndex[PortIndex] = NetworkIndex;
	}
	//Networks[NetworkIndex].AngVel = SumProduct / Volume;
	//Networks[NetworkIndex].MOI = Volume;
}

void USLMDomainAir::DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex)
{
	const FSLMDataAir NetworkData = Networks[NetworkIndex];
	FSLMDataAir& PortData = Ports[PortIndex].DefaultData;
	
	PortData.Pressure_atm = NetworkData.Pressure_atm;
	PortData.Temp_K = NetworkData.Temp_K;
	//PortData.Oxygen = NetworkData.Oxygen;
}

void USLMDomainAir::RemovePortAtIndex(const int32 PortIndex)
{
	Ports.RemoveAt(PortIndex);
}

void USLMDomainAir::RemoveNetworkAtIndex(const int32 NetworkIndex)
{
	Networks.RemoveAt(NetworkIndex);
}

void USLMDomainAir::CreateNetworkForPort(const int32 Port)
{
	PortIndexToNetworkIndex[Port] = Networks.Add(Ports[Port].DefaultData);
}
