#pragma once

#include "CoreMinimal.h"
#include "JungleWorldTerrainBiomeRunway.h"
#include "JungleWorldMaskData.h"

namespace JungleWorldDebugData
{
	struct FWorldDebugSample
	{
		FName Id;
		FVector Location = FVector::ZeroVector;
		JungleWorldTerrainBiomeRunway::EEvidenceView View = JungleWorldTerrainBiomeRunway::EEvidenceView::Overview;
		JungleWorldMaskData::FMaskSample Terrain;
		JungleWorldMaskData::FBiomeMaskSample Biome;
	};
}
