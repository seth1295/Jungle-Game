#pragma once

#include "CoreMinimal.h"

namespace JungleWorldTerrainBiomeRunway
{
	enum class ETerrainChannel : uint8
	{
		Height,
		Slope,
		Aspect,
		Ridge,
		Valley,
		Flow,
		CoastDistance,
		CreekDistance,
		Canopy,
		Gap
	};

	enum class EBiomeOutput : uint8
	{
		CreekRiparian,
		WetValley,
		RidgeCrest,
		SpurShoulder,
		LowlandAlluvial,
		GorgeGully,
		CoastTransition,
		CanopyGap,
		DenseFriction,
		MisleadingClearing
	};

	enum class ERouteClass : uint8
	{
		CreekFollow,
		RidgeWalk,
		SpurDescent,
		SaddleCross,
		BasinCross,
		CoastMove,
		LowlandPush,
		GullySqueeze,
		SlopeTraverse,
		CanopyGapOrient
	};

	enum class EEvidenceView : uint8
	{
		Coast,
		Creek,
		Ridge,
		Saddle,
		Lowland,
		Basin,
		CanopyGap,
		SlopeBreak,
		Overview
	};

	struct FTerrainReviewPoint
	{
		FName Id;
		FVector Location = FVector::ZeroVector;
		ETerrainChannel PrimaryChannel = ETerrainChannel::Height;
		EBiomeOutput ExpectedBiome = EBiomeOutput::CreekRiparian;
		ERouteClass ExpectedRoute = ERouteClass::CreekFollow;
		EEvidenceView EvidenceView = EEvidenceView::Overview;
	};

	struct FWorldRunwayBatch
	{
		FName Id;
		int32 SliceStart = 0;
		int32 SliceEnd = 0;
		FName RequiredHandoff;
	};

	const TCHAR* LexToString(ETerrainChannel Channel);
	const TCHAR* LexToString(EBiomeOutput Output);
	const TCHAR* LexToString(ERouteClass Route);
	const TCHAR* LexToString(EEvidenceView View);
}
