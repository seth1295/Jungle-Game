#pragma once

#include "CoreMinimal.h"

enum class EJGFullSizeDebugMask : uint8
{
	Height,
	Slope,
	Aspect,
	RidgeValley,
	CreekInfluence,
	CoastDistance,
	Wetness,
	SkyWindow,
	HardBlocker,
	SoftBlocker,
	Affordance,
	FalseAffordance,
	BiomeClass,
};

enum class EJGFullSizeBiomeDebugClass : uint8
{
	Ocean,
	Coast,
	Mangrove,
	CreekMouth,
	RiparianWetValley,
	InteriorRainforest,
	RidgeForest,
	MountainShoulder,
	BasinLowland,
	AlluvialFlat,
	Gorge,
};

enum class EJGFullSizeTraversalDebugClass : uint8
{
	Open,
	SlowVegetation,
	WetSlow,
	SteepSlow,
	Scramble,
	HardBlocked,
	FalseAffordance,
};

struct FJGFullSizeMaskBiomeReviewSample
{
	FName SampleId;
	FName ReviewPointId;
	EJGFullSizeBiomeDebugClass Biome = EJGFullSizeBiomeDebugClass::InteriorRainforest;
	EJGFullSizeTraversalDebugClass Traversal = EJGFullSizeTraversalDebugClass::Open;
	float HeightMeters = 0.0f;
	float SlopeDegrees = 0.0f;
	float CreekInfluence = 0.0f;
	float CoastDistanceMeters = 0.0f;
	float Wetness = 0.0f;
	bool bDeveloperOnly = true;
};

struct FJGFullSizeMaskBiomeDebugSpec
{
	static constexpr int32 DebugSpecVersion = 1;

	static bool AllowsPlayerFacingDebugView()
	{
		return false;
	}

	static bool RequiresTerrainDerivedClassification()
	{
		return true;
	}

	static TArray<FName> RequiredMaskNames()
	{
		return {
			TEXT("height"),
			TEXT("slope"),
			TEXT("aspect"),
			TEXT("ridge_valley"),
			TEXT("creek_influence"),
			TEXT("coast_distance"),
			TEXT("wetness"),
			TEXT("sky_window"),
			TEXT("hard_blocker"),
			TEXT("soft_blocker"),
			TEXT("affordance"),
			TEXT("false_affordance"),
			TEXT("biome_class"),
		};
	}

	static TArray<FJGFullSizeMaskBiomeReviewSample> InitialReviewSamples()
	{
		return {
			{ TEXT("FMB_Coast"), TEXT("Review_Coast_North"), EJGFullSizeBiomeDebugClass::Coast, EJGFullSizeTraversalDebugClass::Open, 4.0f, 3.0f, 0.1f, 80.0f, 0.3f, true },
			{ TEXT("FMB_Creek"), TEXT("Review_Creek_Valley"), EJGFullSizeBiomeDebugClass::RiparianWetValley, EJGFullSizeTraversalDebugClass::WetSlow, 36.0f, 5.0f, 1.0f, 3600.0f, 0.9f, true },
			{ TEXT("FMB_Ridge"), TEXT("Review_Ridge_Spine"), EJGFullSizeBiomeDebugClass::RidgeForest, EJGFullSizeTraversalDebugClass::SteepSlow, 420.0f, 26.0f, 0.0f, 5200.0f, 0.2f, true },
			{ TEXT("FMB_Basin"), TEXT("Review_Basin_Lowland"), EJGFullSizeBiomeDebugClass::BasinLowland, EJGFullSizeTraversalDebugClass::SlowVegetation, 82.0f, 2.0f, 0.3f, 4200.0f, 0.7f, true },
			{ TEXT("FMB_Gorge"), TEXT("Review_Gorge_Candidate"), EJGFullSizeBiomeDebugClass::Gorge, EJGFullSizeTraversalDebugClass::Scramble, 160.0f, 34.0f, 0.7f, 5000.0f, 0.6f, true },
		};
	}
};
