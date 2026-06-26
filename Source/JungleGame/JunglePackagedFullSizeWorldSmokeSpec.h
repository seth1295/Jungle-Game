#pragma once

#include "CoreMinimal.h"

enum class EJGFullSizeWorldSmokePoint : uint8
{
	CoastNorth,
	CoastCreekMouth,
	RidgeSpine,
	CreekValley,
	BasinLowland,
	HighlandShoulder,
	MountainLookout,
	Cell0BridgeAnchor,
};

enum class EJGFullSizeWorldSmokeResult : uint8
{
	NotRun,
	Pass,
	Fail,
	Blocked,
};

struct FJGFullSizeWorldSmokePointSpec
{
	FName StableName;
	EJGFullSizeWorldSmokePoint Point = EJGFullSizeWorldSmokePoint::BasinLowland;
	FName ExpectedEvidence;
	bool bRequiresDownwardTerrainHit = true;
	bool bRequiresNoPlayerHud = true;
	bool bDeveloperOnly = true;
};

struct FJGPackagedFullSizeWorldSmokeSpec
{
	static constexpr int32 SmokeSpecVersion = 1;

	static FName SmokeId()
	{
		return TEXT("JG_FULL_WORLD_PACKAGE_SMOKE_002");
	}

	static FName IntendedWorldAssetPath()
	{
		return TEXT("/Game/JungleGame/Maps/WLD_JungleIsland_Full");
	}

	static FName CurrentBridgeMapPath()
	{
		return TEXT("/Engine/Maps/Templates/OpenWorld");
	}

	static FName RequiredRuntimeLogToken()
	{
		return TEXT("Full-size terrain shell spawned");
	}

	static bool CommitsGeneratedScreenshots()
	{
		return false;
	}

	static bool AllowsPlayerFacingEvidenceUI()
	{
		return false;
	}

	static TArray<FJGFullSizeWorldSmokePointSpec> RequiredSmokePoints()
	{
		return {
			{ TEXT("Review_Coast_North"), EJGFullSizeWorldSmokePoint::CoastNorth, TEXT("shoreline_and_ocean_boundary"), true, true, true },
			{ TEXT("Review_Coast_CreekMouth"), EJGFullSizeWorldSmokePoint::CoastCreekMouth, TEXT("creek_to_coast_transition"), true, true, true },
			{ TEXT("Review_Ridge_Spine"), EJGFullSizeWorldSmokePoint::RidgeSpine, TEXT("high_ground_orientation_structure"), true, true, true },
			{ TEXT("Review_Creek_Valley"), EJGFullSizeWorldSmokePoint::CreekValley, TEXT("low_channel_corridor"), true, true, true },
			{ TEXT("Review_Basin_Lowland"), EJGFullSizeWorldSmokePoint::BasinLowland, TEXT("safe_lowland_start"), true, true, true },
			{ TEXT("Review_Highland_Shoulder"), EJGFullSizeWorldSmokePoint::HighlandShoulder, TEXT("lowland_to_highland_transition"), true, true, true },
			{ TEXT("Review_Mountain_Lookout"), EJGFullSizeWorldSmokePoint::MountainLookout, TEXT("world_scale_visibility"), true, true, true },
			{ TEXT("Review_Cell0_BridgeAnchor"), EJGFullSizeWorldSmokePoint::Cell0BridgeAnchor, TEXT("legacy_anchor_only"), true, true, true },
		};
	}
};
