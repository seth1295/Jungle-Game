#include "JungleWorldTerrainBiomeRunway.h"

namespace JungleWorldTerrainBiomeRunway
{
	const TCHAR* LexToString(ETerrainChannel Channel)
	{
		switch (Channel)
		{
		case ETerrainChannel::Height: return TEXT("Height");
		case ETerrainChannel::Slope: return TEXT("Slope");
		case ETerrainChannel::Aspect: return TEXT("Aspect");
		case ETerrainChannel::Ridge: return TEXT("Ridge");
		case ETerrainChannel::Valley: return TEXT("Valley");
		case ETerrainChannel::Flow: return TEXT("Flow");
		case ETerrainChannel::CoastDistance: return TEXT("CoastDistance");
		case ETerrainChannel::CreekDistance: return TEXT("CreekDistance");
		case ETerrainChannel::Canopy: return TEXT("Canopy");
		case ETerrainChannel::Gap: return TEXT("Gap");
		default: return TEXT("Unknown");
		}
	}

	const TCHAR* LexToString(EBiomeOutput Output)
	{
		switch (Output)
		{
		case EBiomeOutput::CreekRiparian: return TEXT("CreekRiparian");
		case EBiomeOutput::WetValley: return TEXT("WetValley");
		case EBiomeOutput::RidgeCrest: return TEXT("RidgeCrest");
		case EBiomeOutput::SpurShoulder: return TEXT("SpurShoulder");
		case EBiomeOutput::LowlandAlluvial: return TEXT("LowlandAlluvial");
		case EBiomeOutput::GorgeGully: return TEXT("GorgeGully");
		case EBiomeOutput::CoastTransition: return TEXT("CoastTransition");
		case EBiomeOutput::CanopyGap: return TEXT("CanopyGap");
		case EBiomeOutput::DenseFriction: return TEXT("DenseFriction");
		case EBiomeOutput::MisleadingClearing: return TEXT("MisleadingClearing");
		default: return TEXT("Unknown");
		}
	}

	const TCHAR* LexToString(ERouteClass Route)
	{
		switch (Route)
		{
		case ERouteClass::CreekFollow: return TEXT("CreekFollow");
		case ERouteClass::RidgeWalk: return TEXT("RidgeWalk");
		case ERouteClass::SpurDescent: return TEXT("SpurDescent");
		case ERouteClass::SaddleCross: return TEXT("SaddleCross");
		case ERouteClass::BasinCross: return TEXT("BasinCross");
		case ERouteClass::CoastMove: return TEXT("CoastMove");
		case ERouteClass::LowlandPush: return TEXT("LowlandPush");
		case ERouteClass::GullySqueeze: return TEXT("GullySqueeze");
		case ERouteClass::SlopeTraverse: return TEXT("SlopeTraverse");
		case ERouteClass::CanopyGapOrient: return TEXT("CanopyGapOrient");
		default: return TEXT("Unknown");
		}
	}

	const TCHAR* LexToString(EEvidenceView View)
	{
		switch (View)
		{
		case EEvidenceView::Coast: return TEXT("Coast");
		case EEvidenceView::Creek: return TEXT("Creek");
		case EEvidenceView::Ridge: return TEXT("Ridge");
		case EEvidenceView::Saddle: return TEXT("Saddle");
		case EEvidenceView::Lowland: return TEXT("Lowland");
		case EEvidenceView::Basin: return TEXT("Basin");
		case EEvidenceView::CanopyGap: return TEXT("CanopyGap");
		case EEvidenceView::SlopeBreak: return TEXT("SlopeBreak");
		case EEvidenceView::Overview: return TEXT("Overview");
		default: return TEXT("Unknown");
		}
	}
}
