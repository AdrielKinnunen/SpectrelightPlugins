// Copyright Spectrelight Studios, LLC. All Rights Reserved.

#include "SLMSystemBase.h"

void FSLMSystemBase::CheckForCleanUp()
{
	if (bNeedsCleanUp)
	{
		CleanUp();
		bNeedsCleanUp = false;
	}
}

void FSLMSystemBase::PreSimulate(const float DeltaTime)
{
}

void FSLMSystemBase::Simulate(const float DeltaTime, const float SubstepScalar)
{
}

void FSLMSystemBase::PostSimulate(const float DeltaTime)
{
}

void FSLMSystemBase::DebugDraw()
{
}