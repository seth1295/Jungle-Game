#pragma once

#include "CoreMinimal.h"
#include "JungleWorldTerrainBiomeRunway.h"

namespace JungleWorldEvidenceSpec
{
	struct FEvidenceSpec
	{
		FName Id;
		JungleWorldTerrainBiomeRunway::EEvidenceView View = JungleWorldTerrainBiomeRunway::EEvidenceView::Overview;
		JungleWorldTerrainBiomeRunway::ETerrainChannel Channel = JungleWorldTerrainBiomeRunway::ETerrainChannel::Height;
	};
}
