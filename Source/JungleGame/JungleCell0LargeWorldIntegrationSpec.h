#pragma once

#include "CoreMinimal.h"
#include "JungleDayNightWeatherDangerSpec.h"

/** Placement rule for making Cell 0 a world location rather than the world itself. */
enum class EJungleCell0PlacementRule : uint8
{
	PlacedDirectorActor,
	FallbackLargeWorldCoordinate,
	PlayerRelativeDebugOnly,
};

/** Terrain relationship expected around the first playable Cell 0 location. */
enum class EJungleCell0TerrainRelationship : uint8
{
	ShelterAnchor,
	CreekCrossing,
	RainPressure,
	TrailMarkRecovery,
	WatcherLineOfSight,
	EntryExitTerrain,
	KnownSafetySmoke,
};

/** Reviewable Cell 0 integration contract. */
struct FJungleCell0IntegrationRuleContract
{
	EJungleCell0TerrainRelationship Relationship = EJungleCell0TerrainRelationship::ShelterAnchor;
	TArray<FString> RequiredMaskNames;
	bool bPreservesCell0Loop = true;
	bool bRequiresPlacedWorldContext = true;
	bool bCanUseFallbackSourceCoordinate = true;
	bool bAllowsPlayerRelativeDebugMode = false;
	bool bAllowsMapCompassClockOrObjectiveUI = false;
	bool bAllowsFullStoryExpansion = false;
	bool bAllowsFinalArtPass = false;

	bool IsWorldIntegrationOnly() const
	{
		return bPreservesCell0Loop
			&& bRequiresPlacedWorldContext
			&& !bAllowsMapCompassClockOrObjectiveUI
			&& !bAllowsFullStoryExpansion
			&& !bAllowsFinalArtPass;
	}

	bool IsReviewable() const
	{
		return IsWorldIntegrationOnly() && RequiredMaskNames.Num() > 0;
	}
};

struct FJungleCell0LargeWorldIntegrationSpec
{
	static constexpr int32 Cell0IntegrationVersion = 1;
	static constexpr double FallbackWorldXCentimeters = 204800.0;
	static constexpr double FallbackWorldYCentimeters = -96768.0;
	static constexpr double FallbackWorldZCentimeters = 1200.0;
	static constexpr double FallbackWorldYawDegrees = 34.0;

	static bool AcceptsPlayerMapMinimapCompassBearingGPSObjectiveUI()
	{
		return false;
	}

	static bool AcceptsHUDClockOrExplicitTimeReadout()
	{
		return false;
	}

	static bool AcceptsOrientationConfidenceFeedback()
	{
		return false;
	}

	static bool AcceptsFullStoryExpansion()
	{
		return false;
	}

	static bool AcceptsFinalArtPass()
	{
		return false;
	}

	static bool AcceptsPlayerRelativeAsPrimaryPlacement()
	{
		return false;
	}

	static bool AllowsPlayerRelativeDebugMode()
	{
		return true;
	}

	static FVector FallbackCell0WorldOrigin()
	{
		return FVector(FallbackWorldXCentimeters, FallbackWorldYCentimeters, FallbackWorldZCentimeters);
	}

	static FRotator FallbackCell0WorldRotation()
	{
		return FRotator(0.0f, FallbackWorldYawDegrees, 0.0f);
	}

	static TArray<FString> RequiredMaskNames()
	{
		return {
			TEXT("height"),
			TEXT("slope"),
			TEXT("creek_distance"),
			TEXT("wetness"),
			TEXT("ridge_valley"),
			TEXT("canopy"),
			TEXT("disturbance"),
			TEXT("visibility"),
			TEXT("sky_window"),
			TEXT("sound_propagation"),
			TEXT("footing_risk"),
			TEXT("night_danger"),
			TEXT("hard_blocker"),
			TEXT("soft_blocker"),
			TEXT("affordance_zone"),
			TEXT("false_affordance"),
		};
	}

	static TArray<FJungleCell0IntegrationRuleContract> RuleContracts()
	{
		TArray<FJungleCell0IntegrationRuleContract> Rules;

		FJungleCell0IntegrationRuleContract Shelter;
		Shelter.Relationship = EJungleCell0TerrainRelationship::ShelterAnchor;
		Shelter.RequiredMaskNames = { TEXT("visibility"), TEXT("sky_window"), TEXT("sound_propagation"), TEXT("night_danger") };
		Rules.Add(Shelter);

		FJungleCell0IntegrationRuleContract Creek;
		Creek.Relationship = EJungleCell0TerrainRelationship::CreekCrossing;
		Creek.RequiredMaskNames = { TEXT("creek_distance"), TEXT("wetness"), TEXT("footing_risk"), TEXT("hard_blocker"), TEXT("soft_blocker"), TEXT("affordance_zone") };
		Rules.Add(Creek);

		FJungleCell0IntegrationRuleContract Rain;
		Rain.Relationship = EJungleCell0TerrainRelationship::RainPressure;
		Rain.RequiredMaskNames = { TEXT("canopy"), TEXT("wetness"), TEXT("sound_propagation"), TEXT("visibility"), TEXT("night_danger") };
		Rules.Add(Rain);

		FJungleCell0IntegrationRuleContract Trail;
		Trail.Relationship = EJungleCell0TerrainRelationship::TrailMarkRecovery;
		Trail.RequiredMaskNames = { TEXT("disturbance"), TEXT("visibility"), TEXT("wetness"), TEXT("affordance_zone") };
		Rules.Add(Trail);

		FJungleCell0IntegrationRuleContract Watcher;
		Watcher.Relationship = EJungleCell0TerrainRelationship::WatcherLineOfSight;
		Watcher.RequiredMaskNames = { TEXT("visibility"), TEXT("sky_window"), TEXT("ridge_valley"), TEXT("canopy") };
		Rules.Add(Watcher);

		FJungleCell0IntegrationRuleContract EntryExit;
		EntryExit.Relationship = EJungleCell0TerrainRelationship::EntryExitTerrain;
		EntryExit.RequiredMaskNames = { TEXT("slope"), TEXT("footing_risk"), TEXT("hard_blocker"), TEXT("soft_blocker"), TEXT("false_affordance"), TEXT("affordance_zone") };
		Rules.Add(EntryExit);

		FJungleCell0IntegrationRuleContract Smoke;
		Smoke.Relationship = EJungleCell0TerrainRelationship::KnownSafetySmoke;
		Smoke.RequiredMaskNames = { TEXT("visibility"), TEXT("sky_window"), TEXT("canopy"), TEXT("sound_propagation") };
		Rules.Add(Smoke);

		return Rules;
	}

	static TArray<FString> RequiredDebugOutputNames()
	{
		return {
			TEXT("cell0_placement_mode"),
			TEXT("cell0_world_origin"),
			TEXT("cell0_entry_exit_affordance"),
			TEXT("cell0_creek_crossing_relationship"),
			TEXT("cell0_known_safety_visibility"),
			TEXT("cell0_watcher_los_relationship"),
			TEXT("cell0_forbidden_ui_absence_check"),
		};
	}
};
