#pragma once

#include "CoreMinimal.h"
#include "JungleEcosystemMaskSpec.h"
#include "JungleMacroTerrainSpec.h"
#include "JunglePCGBiomeSpec.h"

/** Lowland, alluvial, gorge, and coastal-transition subzones for future PCG graph assets. */
enum class EJungleLowlandAlluvialGorgeZone : uint8
{
	OutsideLowlandGorgeInfluence,
	LowlandWetFlat,
	AlluvialBench,
	FloodplainSoftBlocker,
	GorgeConstrainedCorridor,
	GorgeSightlineCompression,
	GorgeSoundCompression,
	CreekMouthMangroveTransition,
	CliffCoastTransition,
	IslandEdgeDensityTransition,
	RegrowthGapAffordance,
	HardGorgeExcluded,
	FalseLowlandAffordance,
};

/** Placeholder lowland/gorge evidence cues; these are contracts, not final hydrology, wildlife, or cinematics. */
enum class EJungleLowlandGorgeEvidenceCue : uint8
{
	FlatWetGround,
	AlluvialSedimentShelf,
	FloodDebrisLine,
	MangroveRootMaze,
	CreekMouthTransition,
	CliffCoastDeadEnd,
	CompressedGorgeSound,
	CompressedGorgeSightline,
	DryCreekBedFalseAffordance,
	OldScarRegrowthGap,
	MisleadingClearing,
	BeachRouteEndsInBlocker,
};

/** Reviewable contract for future lowland/alluvial/gorge PCG rules. */
struct FJungleLowlandGorgeRuleContract
{
	EJungleLowlandAlluvialGorgeZone Zone = EJungleLowlandAlluvialGorgeZone::OutsideLowlandGorgeInfluence;
	EJungleLowlandGorgeEvidenceCue EvidenceCue = EJungleLowlandGorgeEvidenceCue::FlatWetGround;
	EJunglePCGSpawnRuleKind RuleKind = EJunglePCGSpawnRuleKind::GroundClutterPlaceholder;
	EJunglePCGTerrainFrictionClass FrictionClass = EJunglePCGTerrainFrictionClass::Open;
	TArray<FString> RequiredMaskNames;
	float MaxInstancesPerHectare = 0.0f;
	float MinimumSpacingMeters = 1.0f;
	bool bCompressesSightlines = false;
	bool bCompressesSound = false;
	bool bCanCreateFalseAffordance = false;
	bool bNotesDensePerformanceRisk = false;
	bool bDeveloperOnly = true;

	bool IsReviewable() const
	{
		return MaxInstancesPerHectare >= 0.0f
			&& MinimumSpacingMeters > 0.0f
			&& RequiredMaskNames.Num() > 0;
	}
};

struct FJungleLowlandAlluvialGorgeEcosystemSpec
{
	static constexpr int32 EcosystemPassVersion = 1;
	static constexpr float LowlandFlatSlopeDegrees = FJunglePCGBiomeFrameworkSpec::LowlandSlopeDegrees;
	static constexpr float AlluvialWetnessThreshold = 0.62f;
	static constexpr float FloodplainSoftBlockerWetness = 0.76f;
	static constexpr float DenseLowlandCanopyThreshold = 0.74f;
	static constexpr float GorgeSideSlopeDegrees = 38.0f;
	static constexpr float GorgeSightlineVisibilityMax = 0.42f;
	static constexpr float GorgeSoundPropagationMin = 0.66f;
	static constexpr float RegrowthGapAffordanceMin = 0.58f;
	static constexpr float CreekMouthInfluenceMeters = 72.0f;
	static constexpr float CoastalTransitionMeters = FJunglePCGBiomeFrameworkSpec::CoastEdgeMeters;
	static constexpr float DensePerformanceRiskInstancesPerHectare = 84.0f;

	static bool AcceptsFinalCinematics()
	{
		return false;
	}

	static bool AcceptsFullHydrologySimulation()
	{
		return false;
	}

	static bool AcceptsFullWildlifeEcology()
	{
		return false;
	}

	static bool AcceptsUnrelatedCell0Polish()
	{
		return false;
	}

	static bool IsInsideLowlandAlluvialInfluence(const FJunglePCGBiomeInputSample& Sample)
	{
		return Sample.SlopeDegrees <= LowlandFlatSlopeDegrees
			|| Sample.GetMaskValue(TEXT("wetness")) >= AlluvialWetnessThreshold
			|| Sample.CoastDistanceMeters <= CoastalTransitionMeters;
	}

	static bool IsInsideGorgeInfluence(const FJunglePCGBiomeInputSample& Sample)
	{
		return Sample.SlopeDegrees >= GorgeSideSlopeDegrees
			&& Sample.GetMaskValue(TEXT("visibility")) <= GorgeSightlineVisibilityMax
			&& Sample.GetMaskValue(TEXT("ridge_valley")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold;
	}

	static bool IsCreekMouthMangroveTransition(const FJunglePCGBiomeInputSample& Sample)
	{
		return Sample.CoastDistanceMeters <= CoastalTransitionMeters
			&& Sample.CreekDistanceMeters <= CreekMouthInfluenceMeters
			&& Sample.GetMaskValue(TEXT("wetness")) >= AlluvialWetnessThreshold
			&& Sample.GetMaskValue(TEXT("canopy")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold;
	}

	static bool IsDenseLowlandPerformanceRisk(const FJungleLowlandGorgeRuleContract& Contract)
	{
		return Contract.bNotesDensePerformanceRisk
			|| Contract.MaxInstancesPerHectare >= DensePerformanceRiskInstancesPerHectare;
	}

	static EJungleLowlandAlluvialGorgeZone ClassifyLowlandGorgeZone(const FJunglePCGBiomeInputSample& Sample)
	{
		if (!IsInsideLowlandAlluvialInfluence(Sample) && !IsInsideGorgeInfluence(Sample))
		{
			return EJungleLowlandAlluvialGorgeZone::OutsideLowlandGorgeInfluence;
		}

		if (Sample.GetMaskValue(TEXT("hard_blocker")) >= FJungleEcosystemMaskSpec::HardBlockerThreshold
			&& (IsInsideGorgeInfluence(Sample) || FJungleMacroTerrainSpec::IsSlopeHardBlocker(Sample.SlopeDegrees)))
		{
			return EJungleLowlandAlluvialGorgeZone::HardGorgeExcluded;
		}

		if (Sample.GetMaskValue(TEXT("false_affordance")) >= FJungleEcosystemMaskSpec::FalseAffordanceThreshold)
		{
			return EJungleLowlandAlluvialGorgeZone::FalseLowlandAffordance;
		}

		if (IsCreekMouthMangroveTransition(Sample))
		{
			return EJungleLowlandAlluvialGorgeZone::CreekMouthMangroveTransition;
		}

		if (Sample.CoastDistanceMeters <= CoastalTransitionMeters
			&& Sample.SlopeDegrees >= FJungleMacroTerrainSpec::SoftBlockerSlopeDegrees)
		{
			return EJungleLowlandAlluvialGorgeZone::CliffCoastTransition;
		}

		if (IsInsideGorgeInfluence(Sample)
			&& Sample.GetMaskValue(TEXT("sound_propagation")) >= GorgeSoundPropagationMin)
		{
			return EJungleLowlandAlluvialGorgeZone::GorgeSoundCompression;
		}

		if (IsInsideGorgeInfluence(Sample))
		{
			return EJungleLowlandAlluvialGorgeZone::GorgeConstrainedCorridor;
		}

		if (Sample.GetMaskValue(TEXT("visibility")) <= GorgeSightlineVisibilityMax
			&& Sample.GetMaskValue(TEXT("ridge_valley")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold)
		{
			return EJungleLowlandAlluvialGorgeZone::GorgeSightlineCompression;
		}

		if (Sample.GetMaskValue(TEXT("wetness")) >= FloodplainSoftBlockerWetness
			&& Sample.GetMaskValue(TEXT("canopy")) >= DenseLowlandCanopyThreshold)
		{
			return EJungleLowlandAlluvialGorgeZone::FloodplainSoftBlocker;
		}

		if (Sample.GetMaskValue(TEXT("disturbance")) >= RegrowthGapAffordanceMin
			&& Sample.GetMaskValue(TEXT("affordance_zone")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold)
		{
			return EJungleLowlandAlluvialGorgeZone::RegrowthGapAffordance;
		}

		if (Sample.SlopeDegrees <= LowlandFlatSlopeDegrees
			&& Sample.GetMaskValue(TEXT("wetness")) >= AlluvialWetnessThreshold)
		{
			return EJungleLowlandAlluvialGorgeZone::LowlandWetFlat;
		}

		if (Sample.GetMaskValue(TEXT("wetness")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold
			&& Sample.GetMaskValue(TEXT("affordance_zone")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold)
		{
			return EJungleLowlandAlluvialGorgeZone::AlluvialBench;
		}

		return EJungleLowlandAlluvialGorgeZone::IslandEdgeDensityTransition;
	}

	static EJunglePCGTerrainFrictionClass ResolveLowlandGorgeFriction(const FJunglePCGBiomeInputSample& Sample)
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
			TEXT("creek_distance"),
			TEXT("coast_distance"),
			TEXT("wetness"),
			TEXT("ridge_valley"),
			TEXT("canopy"),
			TEXT("disturbance"),
			TEXT("sound_propagation"),
			TEXT("visibility"),
			TEXT("footing_risk"),
			TEXT("hard_blocker"),
			TEXT("soft_blocker"),
			TEXT("affordance_zone"),
			TEXT("false_affordance"),
		};
	}

	static TArray<FJungleLowlandGorgeRuleContract> PlaceholderRuleContracts()
	{
		TArray<FJungleLowlandGorgeRuleContract> Rules;

		FJungleLowlandGorgeRuleContract LowlandWetFlat;
		LowlandWetFlat.Zone = EJungleLowlandAlluvialGorgeZone::LowlandWetFlat;
		LowlandWetFlat.EvidenceCue = EJungleLowlandGorgeEvidenceCue::FlatWetGround;
		LowlandWetFlat.RuleKind = EJunglePCGSpawnRuleKind::GroundClutterPlaceholder;
		LowlandWetFlat.FrictionClass = EJunglePCGTerrainFrictionClass::ReadablePlantFriction;
		LowlandWetFlat.RequiredMaskNames = { TEXT("slope"), TEXT("wetness"), TEXT("footing_risk") };
		LowlandWetFlat.MaxInstancesPerHectare = 56.0f;
		LowlandWetFlat.MinimumSpacingMeters = 4.0f;
		Rules.Add(LowlandWetFlat);

		FJungleLowlandGorgeRuleContract Floodplain;
		Floodplain.Zone = EJungleLowlandAlluvialGorgeZone::FloodplainSoftBlocker;
		Floodplain.EvidenceCue = EJungleLowlandGorgeEvidenceCue::FloodDebrisLine;
		Floodplain.RuleKind = EJunglePCGSpawnRuleKind::DeadfallPlaceholder;
		Floodplain.FrictionClass = EJunglePCGTerrainFrictionClass::ReadablePlantFriction;
		Floodplain.RequiredMaskNames = { TEXT("wetness"), TEXT("canopy"), TEXT("soft_blocker"), TEXT("footing_risk") };
		Floodplain.MaxInstancesPerHectare = 88.0f;
		Floodplain.MinimumSpacingMeters = 3.0f;
		Floodplain.bNotesDensePerformanceRisk = true;
		Rules.Add(Floodplain);

		FJungleLowlandGorgeRuleContract MangroveCreekMouth;
		MangroveCreekMouth.Zone = EJungleLowlandAlluvialGorgeZone::CreekMouthMangroveTransition;
		MangroveCreekMouth.EvidenceCue = EJungleLowlandGorgeEvidenceCue::MangroveRootMaze;
		MangroveCreekMouth.RuleKind = EJunglePCGSpawnRuleKind::PlantFrictionPlaceholder;
		MangroveCreekMouth.FrictionClass = EJunglePCGTerrainFrictionClass::HeavyPlantFriction;
		MangroveCreekMouth.RequiredMaskNames = { TEXT("coast_distance"), TEXT("creek_distance"), TEXT("wetness"), TEXT("canopy") };
		MangroveCreekMouth.MaxInstancesPerHectare = 72.0f;
		MangroveCreekMouth.MinimumSpacingMeters = 4.0f;
		MangroveCreekMouth.bNotesDensePerformanceRisk = true;
		Rules.Add(MangroveCreekMouth);

		FJungleLowlandGorgeRuleContract GorgeCorridor;
		GorgeCorridor.Zone = EJungleLowlandAlluvialGorgeZone::GorgeConstrainedCorridor;
		GorgeCorridor.EvidenceCue = EJungleLowlandGorgeEvidenceCue::CompressedGorgeSightline;
		GorgeCorridor.RuleKind = EJunglePCGSpawnRuleKind::RockRootPlaceholder;
		GorgeCorridor.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		GorgeCorridor.RequiredMaskNames = { TEXT("slope"), TEXT("ridge_valley"), TEXT("visibility"), TEXT("hard_blocker") };
		GorgeCorridor.MaxInstancesPerHectare = 18.0f;
		GorgeCorridor.MinimumSpacingMeters = 10.0f;
		GorgeCorridor.bCompressesSightlines = true;
		Rules.Add(GorgeCorridor);

		FJungleLowlandGorgeRuleContract GorgeSound;
		GorgeSound.Zone = EJungleLowlandAlluvialGorgeZone::GorgeSoundCompression;
		GorgeSound.EvidenceCue = EJungleLowlandGorgeEvidenceCue::CompressedGorgeSound;
		GorgeSound.RuleKind = EJunglePCGSpawnRuleKind::SoundAnchorPlaceholder;
		GorgeSound.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		GorgeSound.RequiredMaskNames = { TEXT("sound_propagation"), TEXT("visibility"), TEXT("ridge_valley") };
		GorgeSound.MaxInstancesPerHectare = 10.0f;
		GorgeSound.MinimumSpacingMeters = 18.0f;
		GorgeSound.bCompressesSound = true;
		GorgeSound.bCompressesSightlines = true;
		Rules.Add(GorgeSound);

		FJungleLowlandGorgeRuleContract FalseAffordance;
		FalseAffordance.Zone = EJungleLowlandAlluvialGorgeZone::FalseLowlandAffordance;
		FalseAffordance.EvidenceCue = EJungleLowlandGorgeEvidenceCue::DryCreekBedFalseAffordance;
		FalseAffordance.RuleKind = EJunglePCGSpawnRuleKind::FalseAffordanceCuePlaceholder;
		FalseAffordance.FrictionClass = EJunglePCGTerrainFrictionClass::FalseAffordance;
		FalseAffordance.RequiredMaskNames = { TEXT("false_affordance"), TEXT("wetness"), TEXT("soft_blocker"), TEXT("visibility") };
		FalseAffordance.MaxInstancesPerHectare = 14.0f;
		FalseAffordance.MinimumSpacingMeters = 12.0f;
		FalseAffordance.bCanCreateFalseAffordance = true;
		Rules.Add(FalseAffordance);

		return Rules;
	}

	static TArray<FString> RequiredValidationOutputNames()
	{
		return {
			TEXT("lowland_alluvial_gorge_zone"),
			TEXT("alluvial_wetness_flatness"),
			TEXT("floodplain_soft_blocker"),
			TEXT("gorge_constrained_corridor"),
			TEXT("gorge_sightline_compression"),
			TEXT("gorge_sound_compression"),
			TEXT("coastal_transition_density"),
			TEXT("mangrove_creek_mouth_placeholder"),
			TEXT("false_lowland_affordance"),
			TEXT("dense_lowland_performance_risk"),
		};
	}
};
