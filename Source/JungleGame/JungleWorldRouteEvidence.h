#pragma once

#include "CoreMinimal.h"
#include "JungleWorldTerrainBiomeRunway.h"

namespace JungleWorldRouteEvidence
{
	struct FRouteEvidenceRule
	{
		JungleWorldTerrainBiomeRunway::ERouteClass RouteClass = JungleWorldTerrainBiomeRunway::ERouteClass::CreekFollow;
		JungleWorldTerrainBiomeRunway::ETerrainChannel PrimaryChannel = JungleWorldTerrainBiomeRunway::ETerrainChannel::CreekDistance;
		JungleWorldTerrainBiomeRunway::EBiomeOutput ExpectedBiome = JungleWorldTerrainBiomeRunway::EBiomeOutput::CreekRiparian;
		JungleWorldTerrainBiomeRunway::EEvidenceView EvidenceView = JungleWorldTerrainBiomeRunway::EEvidenceView::Creek;
	};
}
