#pragma once

#include "CoreMinimal.h"
#include "JungleEcosystemMaskSpec.h"
#include "JungleMacroTerrainSpec.h"
#include "JunglePCGBiomeSpec.h"

/** Ridge, slope, mountain-shoulder, and steep-slope subzones for future PCG graph assets. */
enum class EJungleRidgeSlopeMountainZone : uint8
{
	OutsideRidgeSlopeInfluence,
	RidgeCrestSkyWindow,
	SpurMovementAffordance,
	SaddleTransitionAffordance,
	SlopeBreakRouteDecision,
	MountainShoulderSilhouette,
	ExposedOpenCanopySlope,
	ShelteredDenseSlope,
	SteepSlopeFatigueRisk,
	HardSlopeExcluded,
	FalseSkylineAffordance,
};

/** Placeholder ridge and mountain evidence cues; these are environmental contracts, not UI signage. */
enum class EJungleRidgeSlopeEvidenceCue : uint8
{
	SkyWindow,
	SkylineSilhouette,
	WindOpenCanopy,
	ExposedAspectVegetation,
	ShelteredAspectVegetation,
	ReadableSlopeBreak,
	SpurLineMovement,
	SaddleDipTransition,
	LooseRockRootFooting,
	FatigueRouteReconsideration,
	FalseOpenRidgeLine,
};

/** Reviewable contract for future ridge/slope/mountain PCG rules. */
struct FJungleRidgeSlopeRuleContract
{
	EJungleRidgeSlopeMountainZone Zone = EJungleRidgeSlopeMountainZone::OutsideRidgeSlopeInfluence;
	EJungleRidgeSlopeEvidenceCue EvidenceCue = EJungleRidgeSlopeEvidenceCue::ReadableSlopeBreak;
	EJunglePCGSpawnRuleKind RuleKind = EJunglePCGSpawnRuleKind::GroundClutterPlaceholder;
	EJunglePCGTerrainFrictionClass FrictionClass = EJunglePCGTerrainFrictionClass::Open;
	TArray<FString> RequiredMaskNames;
	float MaxInstancesPerHectare = 0.0f;
	float MinimumSpacingMeters = 1.0f;
	bool bPreservesSkyWindow = false;
	bool bCanAnchorSilhouette = false;
	bool bRequiresFutureHLODReview = false;
	bool bDeveloperOnly = true;

	bool IsReviewable() const
	{
		return MaxInstancesPerHectare >= 0.0f
			&& MinimumSpacingMeters > 0.0f
			&& RequiredMaskNames.Num() > 0;
	}
};

struct FJungleRidgeSlopeMountainEcosystemSpec
{
	static constexpr int32 EcosystemPassVersion = 1;
	static constexpr float MinimumRidgeInfluence = FJunglePCGBiomeFrameworkSpec::RidgeInfluenceScore;
	static constexpr float MinimumSkyWindow = 0.62f;
	static constexpr float MinimumSilhouetteVisibility = 0.68f;
	static constexpr float MinimumSpurAffordance = 0.64f;
	static constexpr float MinimumSaddleAffordance = 0.58f;
	static constexpr float MinimumOpenCanopyWindCue = 0.56f;
	static constexpr float MinimumMountainShoulderHeightMeters = 240.0f;
	static constexpr float MaximumOpenCanopyMask = 0.44f;
	static constexpr float ShelteredDenseCanopyMask = 0.72f;
	static constexpr float ExposedAspectLowerBand = 0.0f;
	static constexpr float ExposedAspectUpperBand = 0.66f;
	static constexpr float ShelteredAspectLowerBand = 0.32f;
	static constexpr float ShelteredAspectUpperBand = 0.58f;
	static constexpr float FatigueSlopeDegrees = 34.0f;
	static constexpr float InjuryRiskFooting = 0.70f;

	static bool AcceptsClimbingSystem()
	{
		return false;
	}

	static bool AcceptsFinalMountainArt()
	{
		return false;
	}

	static bool AcceptsFullWeatherSimulation()
	{
		return false;
	}

	static bool AcceptsArtificialLandmarkSignage()
	{
		return false;
	}

	static bool IsExposedAspect(float NormalizedAspect)
	{
		return NormalizedAspect <= ExposedAspectLowerBand + 0.12f
			|| NormalizedAspect >= ExposedAspectUpperBand;
	}

	static bool IsShelteredAspect(float NormalizedAspect)
	{
		return NormalizedAspect >= ShelteredAspectLowerBand
			&& NormalizedAspect <= ShelteredAspectUpperBand;
	}

	static bool IsInsideRidgeSlopeInfluence(const FJunglePCGBiomeInputSample& Sample)
	{
		return Sample.GetMaskValue(TEXT("ridge_valley")) >= MinimumRidgeInfluence
			|| Sample.SlopeDegrees >= FJungleMacroTerrainSpec::MaximumRoutineWalkableSlopeDegrees
			|| Sample.GetMaskValue(TEXT("sky_window")) >= MinimumSkyWindow;
	}

	static bool PreservesSkyWindow(const FJunglePCGBiomeInputSample& Sample)
	{
		return Sample.GetMaskValue(TEXT("sky_window")) >= MinimumSkyWindow
			&& Sample.GetMaskValue(TEXT("visibility")) >= MinimumSilhouetteVisibility
			&& Sample.GetMaskValue(TEXT("canopy")) <= MaximumOpenCanopyMask;
	}

	static bool IsMountainShoulderSilhouette(const FJunglePCGBiomeInputSample& Sample)
	{
		return Sample.HeightMeters >= MinimumMountainShoulderHeightMeters
			&& Sample.GetMaskValue(TEXT("ridge_valley")) >= MinimumRidgeInfluence
			&& Sample.GetMaskValue(TEXT("visibility")) >= MinimumSilhouetteVisibility;
	}

	static bool IsSteepSlopeFatigueRisk(const FJunglePCGBiomeInputSample& Sample)
	{
		return Sample.SlopeDegrees >= FatigueSlopeDegrees
			|| Sample.GetMaskValue(TEXT("footing_risk")) >= InjuryRiskFooting;
	}

	static EJungleRidgeSlopeMountainZone ClassifyRidgeSlopeZone(const FJunglePCGBiomeInputSample& Sample)
	{
		if (!IsInsideRidgeSlopeInfluence(Sample))
		{
			return EJungleRidgeSlopeMountainZone::OutsideRidgeSlopeInfluence;
		}

		if (Sample.GetMaskValue(TEXT("hard_blocker")) >= FJungleEcosystemMaskSpec::HardBlockerThreshold
			|| FJungleMacroTerrainSpec::IsSlopeHardBlocker(Sample.SlopeDegrees))
		{
			return EJungleRidgeSlopeMountainZone::HardSlopeExcluded;
		}

		if (Sample.GetMaskValue(TEXT("false_affordance")) >= FJungleEcosystemMaskSpec::FalseAffordanceThreshold)
		{
			return EJungleRidgeSlopeMountainZone::FalseSkylineAffordance;
		}

		if (PreservesSkyWindow(Sample))
		{
			return EJungleRidgeSlopeMountainZone::RidgeCrestSkyWindow;
		}

		if (IsMountainShoulderSilhouette(Sample))
		{
			return EJungleRidgeSlopeMountainZone::MountainShoulderSilhouette;
		}

		if (IsSteepSlopeFatigueRisk(Sample))
		{
			return EJungleRidgeSlopeMountainZone::SteepSlopeFatigueRisk;
		}

		const float NormalizedAspect = Sample.GetMaskValue(TEXT("aspect"));
		if (IsExposedAspect(NormalizedAspect)
			&& Sample.GetMaskValue(TEXT("canopy")) <= MaximumOpenCanopyMask
			&& Sample.GetMaskValue(TEXT("sound_propagation")) >= MinimumOpenCanopyWindCue)
		{
			return EJungleRidgeSlopeMountainZone::ExposedOpenCanopySlope;
		}

		if (IsShelteredAspect(NormalizedAspect)
			&& Sample.GetMaskValue(TEXT("canopy")) >= ShelteredDenseCanopyMask)
		{
			return EJungleRidgeSlopeMountainZone::ShelteredDenseSlope;
		}

		if (Sample.GetMaskValue(TEXT("ridge_valley")) >= MinimumRidgeInfluence
			&& Sample.GetMaskValue(TEXT("affordance_zone")) >= MinimumSpurAffordance)
		{
			return EJungleRidgeSlopeMountainZone::SpurMovementAffordance;
		}

		if (Sample.GetMaskValue(TEXT("affordance_zone")) >= MinimumSaddleAffordance
			&& Sample.SlopeDegrees <= FJungleMacroTerrainSpec::MaximumRoutineWalkableSlopeDegrees)
		{
			return EJungleRidgeSlopeMountainZone::SaddleTransitionAffordance;
		}

		return EJungleRidgeSlopeMountainZone::SlopeBreakRouteDecision;
	}

	static EJunglePCGTerrainFrictionClass ResolveSlopeFriction(const FJunglePCGBiomeInputSample& Sample)
	{
		return FJunglePCGBiomeFrameworkSpec::ResolveTerrainFriction(
			Sample.GetMaskValue(TEXT("hard_blocker")),
			Sample.GetMaskValue(TEXT("soft_blocker")),
			Sample.GetMaskValue(TEXT("affordance_zone")),
			Sample.GetMaskValue(TEXT("false_affordance")));
	}

	static TArray<FString> RequiredMaskNames()
	{
		return {
			TEXT("height"),
			TEXT("slope"),
			TEXT("aspect"),
			TEXT("ridge_valley"),
			TEXT("sky_window"),
			TEXT("visibility"),
			TEXT("canopy"),
			TEXT("sound_propagation"),
			TEXT("footing_risk"),
			TEXT("hard_blocker"),
			TEXT("soft_blocker"),
			TEXT("affordance_zone"),
			TEXT("false_affordance"),
		};
	}

	static TArray<FJungleRidgeSlopeRuleContract> PlaceholderRuleContracts()
	{
		TArray<FJungleRidgeSlopeRuleContract> Rules;

		FJungleRidgeSlopeRuleContract SkyWindow;
		SkyWindow.Zone = EJungleRidgeSlopeMountainZone::RidgeCrestSkyWindow;
		SkyWindow.EvidenceCue = EJungleRidgeSlopeEvidenceCue::SkyWindow;
		SkyWindow.RuleKind = EJunglePCGSpawnRuleKind::SkyWindowPreservationZone;
		SkyWindow.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		SkyWindow.RequiredMaskNames = { TEXT("ridge_valley"), TEXT("sky_window"), TEXT("visibility"), TEXT("canopy") };
		SkyWindow.MaxInstancesPerHectare = 0.0f;
		SkyWindow.MinimumSpacingMeters = 20.0f;
		SkyWindow.bPreservesSkyWindow = true;
		SkyWindow.bRequiresFutureHLODReview = true;
		Rules.Add(SkyWindow);

		FJungleRidgeSlopeRuleContract Silhouette;
		Silhouette.Zone = EJungleRidgeSlopeMountainZone::MountainShoulderSilhouette;
		Silhouette.EvidenceCue = EJungleRidgeSlopeEvidenceCue::SkylineSilhouette;
		Silhouette.RuleKind = EJunglePCGSpawnRuleKind::LandmarkSilhouettePlaceholder;
		Silhouette.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		Silhouette.RequiredMaskNames = { TEXT("height"), TEXT("ridge_valley"), TEXT("visibility"), TEXT("sky_window") };
		Silhouette.MaxInstancesPerHectare = 3.0f;
		Silhouette.MinimumSpacingMeters = 64.0f;
		Silhouette.bCanAnchorSilhouette = true;
		Silhouette.bRequiresFutureHLODReview = true;
		Rules.Add(Silhouette);

		FJungleRidgeSlopeRuleContract SpurAffordance;
		SpurAffordance.Zone = EJungleRidgeSlopeMountainZone::SpurMovementAffordance;
		SpurAffordance.EvidenceCue = EJungleRidgeSlopeEvidenceCue::SpurLineMovement;
		SpurAffordance.RuleKind = EJunglePCGSpawnRuleKind::AffordanceCuePlaceholder;
		SpurAffordance.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		SpurAffordance.RequiredMaskNames = { TEXT("ridge_valley"), TEXT("slope"), TEXT("affordance_zone"), TEXT("visibility") };
		SpurAffordance.MaxInstancesPerHectare = 18.0f;
		SpurAffordance.MinimumSpacingMeters = 10.0f;
		Rules.Add(SpurAffordance);

		FJungleRidgeSlopeRuleContract ExposedWind;
		ExposedWind.Zone = EJungleRidgeSlopeMountainZone::ExposedOpenCanopySlope;
		ExposedWind.EvidenceCue = EJungleRidgeSlopeEvidenceCue::WindOpenCanopy;
		ExposedWind.RuleKind = EJunglePCGSpawnRuleKind::SoundAnchorPlaceholder;
		ExposedWind.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		ExposedWind.RequiredMaskNames = { TEXT("aspect"), TEXT("canopy"), TEXT("sound_propagation"), TEXT("sky_window") };
		ExposedWind.MaxInstancesPerHectare = 10.0f;
		ExposedWind.MinimumSpacingMeters = 18.0f;
		ExposedWind.bPreservesSkyWindow = true;
		Rules.Add(ExposedWind);

		FJungleRidgeSlopeRuleContract SteepFatigue;
		SteepFatigue.Zone = EJungleRidgeSlopeMountainZone::SteepSlopeFatigueRisk;
		SteepFatigue.EvidenceCue = EJungleRidgeSlopeEvidenceCue::FatigueRouteReconsideration;
		SteepFatigue.RuleKind = EJunglePCGSpawnRuleKind::RockRootPlaceholder;
		SteepFatigue.FrictionClass = EJunglePCGTerrainFrictionClass::ReadablePlantFriction;
		SteepFatigue.RequiredMaskNames = { TEXT("slope"), TEXT("footing_risk"), TEXT("soft_blocker"), TEXT("affordance_zone") };
		SteepFatigue.MaxInstancesPerHectare = 24.0f;
		SteepFatigue.MinimumSpacingMeters = 8.0f;
		Rules.Add(SteepFatigue);

		FJungleRidgeSlopeRuleContract FalseRidge;
		FalseRidge.Zone = EJungleRidgeSlopeMountainZone::FalseSkylineAffordance;
		FalseRidge.EvidenceCue = EJungleRidgeSlopeEvidenceCue::FalseOpenRidgeLine;
		FalseRidge.RuleKind = EJunglePCGSpawnRuleKind::FalseAffordanceCuePlaceholder;
		FalseRidge.FrictionClass = EJunglePCGTerrainFrictionClass::FalseAffordance;
		FalseRidge.RequiredMaskNames = { TEXT("false_affordance"), TEXT("ridge_valley"), TEXT("footing_risk") };
		FalseRidge.MaxInstancesPerHectare = 8.0f;
		FalseRidge.MinimumSpacingMeters = 18.0f;
		Rules.Add(FalseRidge);

		return Rules;
	}

	static TArray<FString> RequiredValidationOutputNames()
	{
		return {
			TEXT("ridge_slope_mountain_zone"),
			TEXT("ridge_sky_window_preservation"),
			TEXT("mountain_silhouette_anchor"),
			TEXT("spur_saddle_affordance"),
			TEXT("slope_break_route_decision"),
			TEXT("exposed_sheltered_aspect_variation"),
			TEXT("steep_slope_fatigue_risk"),
			TEXT("false_skyline_affordance"),
		};
	}
};
