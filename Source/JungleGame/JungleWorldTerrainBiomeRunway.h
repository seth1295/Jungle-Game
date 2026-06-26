#pragma once

#include "CoreMinimal.h"

namespace JungleWorldTerrainBiomeRunway
{
	enum class ETerrainChannel : uint8
	{
		Height,
		Slope,
		Aspect,
		Curvature,
		FlowAccumulation,
		CatchmentId,
		TopographicWetness,
		Exposure,
		Landform,
		TraversalResistance,
		RidgeInfluence,
		ValleyInfluence,
		CoastDistance,
		CreekDistance,
		CanopyClosure,
		CanopyGap,
		SkyWindow,
		AudioInfluence
	};

	enum class ELandformClass : uint8
	{
		UplandSpine,
		PrimaryRidge,
		Spur,
		Saddle,
		CreekHead,
		CreekCorridor,
		WetValley,
		Gully,
		Gorge,
		Basin,
		LowlandAlluvial,
		CoastBeach,
		RockyHeadland,
		MangroveTransition,
		CliffCoast,
		CanopyGap
	};

	enum class ETraversalCategory : uint8
	{
		HardBlocker,
		SoftBlocker,
		Affordance,
		FalseAffordance,
		NeutralFriction
	};

	enum class ETerrainEvidenceType : uint8
	{
		SkylineForm,
		SlopeBreak,
		WaterDirection,
		RidgeLine,
		SaddleCrossing,
		CreekSound,
		CoastSound,
		CanopyWindow,
		FootingChange,
		VegetationTransition,
		FalseClearing
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
		Gorge,
		CanopyGap,
		SlopeBreak,
		FalseAffordance,
		Overview
	};

	struct FTerrainChannelContract
	{
		ETerrainChannel Channel = ETerrainChannel::Height;
		FName StableName;
		float MinimumValue = 0.0f;
		float MaximumValue = 1.0f;
		bool bNormalized = true;
		bool bDeveloperOnly = true;
	};

	struct FLandformTraversalRule
	{
		ELandformClass Landform = ELandformClass::UplandSpine;
		ETraversalCategory Category = ETraversalCategory::NeutralFriction;
		ETerrainEvidenceType PrimaryEvidence = ETerrainEvidenceType::SkylineForm;
		FName DebugReason;
	};

	struct FTerrainReviewPoint
	{
		FName Id;
		FVector Location = FVector::ZeroVector;
		ELandformClass Landform = ELandformClass::UplandSpine;
		ETerrainChannel PrimaryChannel = ETerrainChannel::Height;
		ETerrainEvidenceType EvidenceType = ETerrainEvidenceType::SkylineForm;
		ETraversalCategory TraversalCategory = ETraversalCategory::NeutralFriction;
		EBiomeOutput ExpectedBiome = EBiomeOutput::CreekRiparian;
		ERouteClass ExpectedRoute = ERouteClass::CreekFollow;
		EEvidenceView EvidenceView = EEvidenceView::Overview;
	};

	struct FMacroIslandTerrainPlan
	{
		FName Id;
		int32 PrimaryCatchmentCount = 7;
		bool bHasUplandSpine = true;
		bool bHasDistinctCoastalOutlets = true;
		bool bUsesDeveloperOnlyEvidence = true;
	};

	struct FWorldRunwayBatch
	{
		FName Id;
		int32 SliceStart = 0;
		int32 SliceEnd = 0;
		FName RequiredHandoff;
	};

	TConstArrayView<FTerrainChannelContract> GetTerrainChannelContracts();
	TConstArrayView<FLandformTraversalRule> GetLandformTraversalRules();
	TConstArrayView<FTerrainReviewPoint> GetDefaultTerrainReviewPoints();

	const TCHAR* LexToString(ETerrainChannel Channel);
	const TCHAR* LexToString(ELandformClass Landform);
	const TCHAR* LexToString(ETraversalCategory Category);
	const TCHAR* LexToString(ETerrainEvidenceType Evidence);
	const TCHAR* LexToString(EBiomeOutput Output);
	const TCHAR* LexToString(ERouteClass Route);
	const TCHAR* LexToString(EEvidenceView View);
}
