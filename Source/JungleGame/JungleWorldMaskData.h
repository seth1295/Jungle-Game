#pragma once

#include "CoreMinimal.h"
#include "JungleWorldTerrainBiomeRunway.h"

namespace JungleWorldMaskData
{
	struct FMaskSample
	{
		FName Id;
		JungleWorldTerrainBiomeRunway::ETerrainChannel Channel = JungleWorldTerrainBiomeRunway::ETerrainChannel::Height;
		float Value = 0.0f;
		float Normalized = 0.0f;
	};

	struct FBiomeMaskSample
	{
		FName Id;
		JungleWorldTerrainBiomeRunway::EBiomeOutput Output = JungleWorldTerrainBiomeRunway::EBiomeOutput::CreekRiparian;
		float Strength = 0.0f;
	};
}
