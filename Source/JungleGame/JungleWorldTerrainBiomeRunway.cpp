#include "JungleWorldTerrainBiomeRunway.h"

namespace JungleWorldTerrainBiomeRunway
{
	namespace
	{
		const FTerrainChannelContract TerrainChannelContracts[] =
		{
			{ ETerrainChannel::Height, TEXT("M_Terrain_HeightMeters"), -500.0f, 2000.0f, false, true },
			{ ETerrainChannel::Slope, TEXT("M_Terrain_SlopeDegrees"), 0.0f, 90.0f, false, true },
			{ ETerrainChannel::Aspect, TEXT("M_Terrain_AspectDegrees"), 0.0f, 360.0f, false, true },
			{ ETerrainChannel::Curvature, TEXT("M_Terrain_Curvature"), -1.0f, 1.0f, true, true },
			{ ETerrainChannel::FlowAccumulation, TEXT("M_Terrain_FlowAccumulation"), 0.0f, 1.0f, true, true },
			{ ETerrainChannel::CatchmentId, TEXT("M_Terrain_CatchmentId"), 0.0f, 16.0f, false, true },
			{ ETerrainChannel::TopographicWetness, TEXT("M_Terrain_TopographicWetness"), 0.0f, 1.0f, true, true },
			{ ETerrainChannel::Exposure, TEXT("M_Terrain_Exposure"), 0.0f, 1.0f, true, true },
			{ ETerrainChannel::Landform, TEXT("M_Terrain_LandformClass"), 0.0f, 32.0f, false, true },
			{ ETerrainChannel::TraversalResistance, TEXT("M_Terrain_TraversalResistance"), 0.0f, 1.0f, true, true },
			{ ETerrainChannel::RidgeInfluence, TEXT("M_Terrain_RidgeInfluence"), 0.0f, 1.0f, true, true },
			{ ETerrainChannel::ValleyInfluence, TEXT("M_Terrain_ValleyInfluence"), 0.0f, 1.0f, true, true },
			{ ETerrainChannel::CoastDistance, TEXT("M_Terrain_CoastDistanceMeters"), 0.0f, 100000.0f, false, true },
			{ ETerrainChannel::CreekDistance, TEXT("M_Terrain_CreekDistanceMeters"), 0.0f, 100000.0f, false, true },
			{ ETerrainChannel::CanopyClosure, TEXT("M_Terrain_CanopyClosure"), 0.0f, 1.0f, true, true },
			{ ETerrainChannel::CanopyGap, TEXT("M_Terrain_CanopyGap"), 0.0f, 1.0f, true, true },
			{ ETerrainChannel::SkyWindow, TEXT("M_Terrain_SkyWindow"), 0.0f, 1.0f, true, true },
			{ ETerrainChannel::AudioInfluence, TEXT("M_Terrain_AudioInfluence"), 0.0f, 1.0f, true, true }
		};

		const FLandformTraversalRule LandformTraversalRules[] =
		{
			{ ELandformClass::UplandSpine, ETraversalCategory::Affordance, ETerrainEvidenceType::SkylineForm, TEXT("Upland spine creates persistent skyline and catchment structure") },
			{ ELandformClass::PrimaryRidge, ETraversalCategory::Affordance, ETerrainEvidenceType::RidgeLine, TEXT("Primary ridge is a readable movement spine") },
			{ ELandformClass::Spur, ETraversalCategory::Affordance, ETerrainEvidenceType::SlopeBreak, TEXT("Spurs support descent/ascent choices without map UI") },
			{ ELandformClass::Saddle, ETraversalCategory::Affordance, ETerrainEvidenceType::SaddleCrossing, TEXT("Saddles are natural crossings between catchments") },
			{ ELandformClass::CreekHead, ETraversalCategory::NeutralFriction, ETerrainEvidenceType::WaterDirection, TEXT("Creek heads provide weak water evidence but not reliable routes") },
			{ ELandformClass::CreekCorridor, ETraversalCategory::Affordance, ETerrainEvidenceType::CreekSound, TEXT("Creek corridors help route finding while remaining imperfect") },
			{ ELandformClass::WetValley, ETraversalCategory::SoftBlocker, ETerrainEvidenceType::FootingChange, TEXT("Wet valleys slow movement through footing and vegetation") },
			{ ELandformClass::Gully, ETraversalCategory::SoftBlocker, ETerrainEvidenceType::SlopeBreak, TEXT("Gullies compress movement and visibility") },
			{ ELandformClass::Gorge, ETraversalCategory::HardBlocker, ETerrainEvidenceType::WaterDirection, TEXT("Gorges are hard blockers or choke points until proven otherwise") },
			{ ELandformClass::Basin, ETraversalCategory::NeutralFriction, ETerrainEvidenceType::VegetationTransition, TEXT("Basins are readable but can confuse direction") },
			{ ELandformClass::LowlandAlluvial, ETraversalCategory::SoftBlocker, ETerrainEvidenceType::FootingChange, TEXT("Lowland alluvial terrain is slower and wetter") },
			{ ELandformClass::CoastBeach, ETraversalCategory::Affordance, ETerrainEvidenceType::CoastSound, TEXT("Beach/coastline gives strong boundary evidence") },
			{ ELandformClass::RockyHeadland, ETraversalCategory::HardBlocker, ETerrainEvidenceType::SkylineForm, TEXT("Rocky headlands create distinctive blockers and silhouettes") },
			{ ELandformClass::MangroveTransition, ETraversalCategory::SoftBlocker, ETerrainEvidenceType::VegetationTransition, TEXT("Mangrove transition is readable but punishing friction") },
			{ ELandformClass::CliffCoast, ETraversalCategory::HardBlocker, ETerrainEvidenceType::CoastSound, TEXT("Cliff coast is a natural boundary, not an invisible wall") },
			{ ELandformClass::CanopyGap, ETraversalCategory::FalseAffordance, ETerrainEvidenceType::FalseClearing, TEXT("Canopy gaps may attract movement but not guarantee passage") }
		};

		const FTerrainReviewPoint DefaultTerrainReviewPoints[] =
		{
			{ TEXT("Review_UplandSpine_Skyline"), FVector::ZeroVector, ELandformClass::UplandSpine, ETerrainChannel::Landform, ETerrainEvidenceType::SkylineForm, ETraversalCategory::Affordance, EBiomeOutput::RidgeCrest, ERouteClass::RidgeWalk, EEvidenceView::Ridge },
			{ TEXT("Review_Saddle_CatchmentCross"), FVector::ZeroVector, ELandformClass::Saddle, ETerrainChannel::RidgeInfluence, ETerrainEvidenceType::SaddleCrossing, ETraversalCategory::Affordance, EBiomeOutput::SpurShoulder, ERouteClass::SaddleCross, EEvidenceView::Saddle },
			{ TEXT("Review_CreekCorridor_WaterDirection"), FVector::ZeroVector, ELandformClass::CreekCorridor, ETerrainChannel::FlowAccumulation, ETerrainEvidenceType::WaterDirection, ETraversalCategory::Affordance, EBiomeOutput::CreekRiparian, ERouteClass::CreekFollow, EEvidenceView::Creek },
			{ TEXT("Review_WetValley_Footing"), FVector::ZeroVector, ELandformClass::WetValley, ETerrainChannel::TopographicWetness, ETerrainEvidenceType::FootingChange, ETraversalCategory::SoftBlocker, EBiomeOutput::WetValley, ERouteClass::LowlandPush, EEvidenceView::Lowland },
			{ TEXT("Review_RockyHeadland_Blocker"), FVector::ZeroVector, ELandformClass::RockyHeadland, ETerrainChannel::CoastDistance, ETerrainEvidenceType::SkylineForm, ETraversalCategory::HardBlocker, EBiomeOutput::CoastTransition, ERouteClass::CoastMove, EEvidenceView::Coast },
			{ TEXT("Review_CanopyGap_FalseAffordance"), FVector::ZeroVector, ELandformClass::CanopyGap, ETerrainChannel::CanopyGap, ETerrainEvidenceType::FalseClearing, ETraversalCategory::FalseAffordance, EBiomeOutput::MisleadingClearing, ERouteClass::CanopyGapOrient, EEvidenceView::FalseAffordance }
		};
	}

	TConstArrayView<FTerrainChannelContract> GetTerrainChannelContracts()
	{
		return TerrainChannelContracts;
	}

	TConstArrayView<FLandformTraversalRule> GetLandformTraversalRules()
	{
		return LandformTraversalRules;
	}

	TConstArrayView<FTerrainReviewPoint> GetDefaultTerrainReviewPoints()
	{
		return DefaultTerrainReviewPoints;
	}

	const TCHAR* LexToString(ETerrainChannel Channel)
	{
		switch (Channel)
		{
		case ETerrainChannel::Height: return TEXT("Height");
		case ETerrainChannel::Slope: return TEXT("Slope");
		case ETerrainChannel::Aspect: return TEXT("Aspect");
		case ETerrainChannel::Curvature: return TEXT("Curvature");
		case ETerrainChannel::FlowAccumulation: return TEXT("FlowAccumulation");
		case ETerrainChannel::CatchmentId: return TEXT("CatchmentId");
		case ETerrainChannel::TopographicWetness: return TEXT("TopographicWetness");
		case ETerrainChannel::Exposure: return TEXT("Exposure");
		case ETerrainChannel::Landform: return TEXT("Landform");
		case ETerrainChannel::TraversalResistance: return TEXT("TraversalResistance");
		case ETerrainChannel::RidgeInfluence: return TEXT("RidgeInfluence");
		case ETerrainChannel::ValleyInfluence: return TEXT("ValleyInfluence");
		case ETerrainChannel::CoastDistance: return TEXT("CoastDistance");
		case ETerrainChannel::CreekDistance: return TEXT("CreekDistance");
		case ETerrainChannel::CanopyClosure: return TEXT("CanopyClosure");
		case ETerrainChannel::CanopyGap: return TEXT("CanopyGap");
		case ETerrainChannel::SkyWindow: return TEXT("SkyWindow");
		case ETerrainChannel::AudioInfluence: return TEXT("AudioInfluence");
		default: return TEXT("Unknown");
		}
	}

	const TCHAR* LexToString(ELandformClass Landform)
	{
		switch (Landform)
		{
		case ELandformClass::UplandSpine: return TEXT("UplandSpine");
		case ELandformClass::PrimaryRidge: return TEXT("PrimaryRidge");
		case ELandformClass::Spur: return TEXT("Spur");
		case ELandformClass::Saddle: return TEXT("Saddle");
		case ELandformClass::CreekHead: return TEXT("CreekHead");
		case ELandformClass::CreekCorridor: return TEXT("CreekCorridor");
		case ELandformClass::WetValley: return TEXT("WetValley");
		case ELandformClass::Gully: return TEXT("Gully");
		case ELandformClass::Gorge: return TEXT("Gorge");
		case ELandformClass::Basin: return TEXT("Basin");
		case ELandformClass::LowlandAlluvial: return TEXT("LowlandAlluvial");
		case ELandformClass::CoastBeach: return TEXT("CoastBeach");
		case ELandformClass::RockyHeadland: return TEXT("RockyHeadland");
		case ELandformClass::MangroveTransition: return TEXT("MangroveTransition");
		case ELandformClass::CliffCoast: return TEXT("CliffCoast");
		case ELandformClass::CanopyGap: return TEXT("CanopyGap");
		default: return TEXT("Unknown");
		}
	}

	const TCHAR* LexToString(ETraversalCategory Category)
	{
		switch (Category)
		{
		case ETraversalCategory::HardBlocker: return TEXT("HardBlocker");
		case ETraversalCategory::SoftBlocker: return TEXT("SoftBlocker");
		case ETraversalCategory::Affordance: return TEXT("Affordance");
		case ETraversalCategory::FalseAffordance: return TEXT("FalseAffordance");
		case ETraversalCategory::NeutralFriction: return TEXT("NeutralFriction");
		default: return TEXT("Unknown");
		}
	}

	const TCHAR* LexToString(ETerrainEvidenceType Evidence)
	{
		switch (Evidence)
		{
		case ETerrainEvidenceType::SkylineForm: return TEXT("SkylineForm");
		case ETerrainEvidenceType::SlopeBreak: return TEXT("SlopeBreak");
		case ETerrainEvidenceType::WaterDirection: return TEXT("WaterDirection");
		case ETerrainEvidenceType::RidgeLine: return TEXT("RidgeLine");
		case ETerrainEvidenceType::SaddleCrossing: return TEXT("SaddleCrossing");
		case ETerrainEvidenceType::CreekSound: return TEXT("CreekSound");
		case ETerrainEvidenceType::CoastSound: return TEXT("CoastSound");
		case ETerrainEvidenceType::CanopyWindow: return TEXT("CanopyWindow");
		case ETerrainEvidenceType::FootingChange: return TEXT("FootingChange");
		case ETerrainEvidenceType::VegetationTransition: return TEXT("VegetationTransition");
		case ETerrainEvidenceType::FalseClearing: return TEXT("FalseClearing");
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
		case EEvidenceView::Gorge: return TEXT("Gorge");
		case EEvidenceView::CanopyGap: return TEXT("CanopyGap");
		case EEvidenceView::SlopeBreak: return TEXT("SlopeBreak");
		case EEvidenceView::FalseAffordance: return TEXT("FalseAffordance");
		case EEvidenceView::Overview: return TEXT("Overview");
		default: return TEXT("Unknown");
		}
	}
}
