#pragma once

#include "CoreMinimal.h"
#include "JungleEcosystemDebugSpec.h"
#include "JungleEcosystemMaskSpec.h"
#include "JungleMacroTerrainSpec.h"
#include "JungleTerrainSourceSpec.h"
#include "JungleWorldPartitionSpec.h"

/** High-level biome graph lanes for later PCG assets and ecosystem passes. */
enum class EJunglePCGBiomeLayer : uint8
{
	IslandEdge,
	CreekRiparianWetValley,
	RidgeSlopeMountain,
	LowlandAlluvialGorge,
	Transition,
};

/** Placeholder output contracts; these are not final foliage or asset-pack bindings. */
enum class EJunglePCGSpawnRuleKind : uint8
{
	CanopyPlaceholder,
	UnderstoryPlaceholder,
	GroundClutterPlaceholder,
	DeadfallPlaceholder,
	RockRootPlaceholder,
	PlantFrictionPlaceholder,
	AffordanceCuePlaceholder,
	FalseAffordanceCuePlaceholder,
	SoundAnchorPlaceholder,
	SkyWindowPreservationZone,
	LandmarkSilhouettePlaceholder,
};

/** Terrain-friction categories consumed by future traversal and ecosystem PCG passes. */
enum class EJunglePCGTerrainFrictionClass : uint8
{
	Open,
	ReadablePlantFriction,
	HeavyPlantFriction,
	FalseAffordance,
	HardExcluded,
};

/** Deterministic terrain and mask sample interface for PCG graph inputs. */
struct FJunglePCGBiomeInputSample
{
	FVector2D WorldMeters = FVector2D::ZeroVector;
	float HeightMeters = 0.0f;
	float SlopeDegrees = 0.0f;
	float CoastDistanceMeters = MAX_flt;
	float CreekDistanceMeters = MAX_flt;
	TMap<FName, float> NormalizedMasks;

	float GetMaskValue(const FName MaskName, float DefaultValue = 0.0f) const
	{
		const float* FoundValue = NormalizedMasks.Find(MaskName);
		return FoundValue ? *FoundValue : DefaultValue;
	}

	bool HasNormalizedMasks() const
	{
		for (const TPair<FName, float>& MaskPair : NormalizedMasks)
		{
			if (!FJungleEcosystemMaskSpec::IsNormalized(MaskPair.Value))
			{
				return false;
			}
		}

		return true;
	}
};

/** Reviewable placeholder spawn-rule contract for later PCG graph assets. */
struct FJunglePCGSpawnRuleContract
{
	EJunglePCGBiomeLayer BiomeLayer = EJunglePCGBiomeLayer::Transition;
	EJunglePCGSpawnRuleKind RuleKind = EJunglePCGSpawnRuleKind::GroundClutterPlaceholder;
	EJunglePCGTerrainFrictionClass FrictionClass = EJunglePCGTerrainFrictionClass::Open;
	TArray<FString> RequiredMaskNames;
	float MaxInstancesPerHectare = 0.0f;
	float MinimumSpacingMeters = 1.0f;
	int32 DeterministicSalt = 0;
	bool bPreservesSkyWindow = false;
	bool bDeveloperDebugOutput = false;

	bool HasDensityBudget() const
	{
		return MaxInstancesPerHectare >= 0.0f && MinimumSpacingMeters > 0.0f;
	}
};

struct FJunglePCGBiomeFrameworkSpec
{
	static constexpr int32 FrameworkVersion = 1;
	static constexpr int32 DefaultDeterministicSeed = 1000;
	static constexpr double RegionScopeMeters = 512.0;
	static constexpr double DenseValidationAreaHectares = 1.0;
	static constexpr float CoastEdgeMeters = 128.0f;
	static constexpr float CreekInfluenceMeters = 96.0f;
	static constexpr float RidgeInfluenceScore = 0.65f;
	static constexpr float LowlandSlopeDegrees = 8.0f;
	static constexpr float HeavyPlantFrictionThreshold = 0.78f;

	static bool MatchesFoundation(int32 SamplesPerSide, double SpacingMeters)
	{
		return FJungleTerrainSourceSpec::MatchesFoundation(SamplesPerSide, SpacingMeters)
			&& SamplesPerSide == FJungleWorldPartitionSpec::HeightmapResolutionVertices;
	}

	static bool AcceptsRuntimePlayerTrailMarksAsGenerationInput()
	{
		return false;
	}

	static EJunglePCGBiomeLayer ClassifyBiomeLayer(const FJunglePCGBiomeInputSample& Sample)
	{
		if (Sample.CoastDistanceMeters <= CoastEdgeMeters)
		{
			return EJunglePCGBiomeLayer::IslandEdge;
		}

		if (Sample.CreekDistanceMeters <= CreekInfluenceMeters
			|| Sample.GetMaskValue(TEXT("wetness")) >= FJungleEcosystemMaskSpec::AffordanceThreshold)
		{
			return EJunglePCGBiomeLayer::CreekRiparianWetValley;
		}

		if (Sample.GetMaskValue(TEXT("ridge_valley")) >= RidgeInfluenceScore
			|| Sample.SlopeDegrees >= FJungleMacroTerrainSpec::MaximumRoutineWalkableSlopeDegrees)
		{
			return EJunglePCGBiomeLayer::RidgeSlopeMountain;
		}

		if (Sample.SlopeDegrees <= LowlandSlopeDegrees
			&& Sample.GetMaskValue(TEXT("wetness")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold)
		{
			return EJunglePCGBiomeLayer::LowlandAlluvialGorge;
		}

		return EJunglePCGBiomeLayer::Transition;
	}

	static EJunglePCGTerrainFrictionClass ResolveTerrainFriction(float HardBlocker, float SoftBlocker, float Affordance, float FalseAffordance)
	{
		const EJungleTraversalMaskClass TraversalClass = FJungleEcosystemMaskSpec::ClassifyTraversal(HardBlocker, SoftBlocker, Affordance, FalseAffordance);

		if (TraversalClass == EJungleTraversalMaskClass::HardBlocker)
		{
			return EJunglePCGTerrainFrictionClass::HardExcluded;
		}

		if (TraversalClass == EJungleTraversalMaskClass::FalseAffordance)
		{
			return EJunglePCGTerrainFrictionClass::FalseAffordance;
		}

		if (TraversalClass == EJungleTraversalMaskClass::SoftBlocker)
		{
			return SoftBlocker >= HeavyPlantFrictionThreshold
				? EJunglePCGTerrainFrictionClass::HeavyPlantFriction
				: EJunglePCGTerrainFrictionClass::ReadablePlantFriction;
		}

		return EJunglePCGTerrainFrictionClass::Open;
	}

	static TArray<FString> RequiredPCGInputNames()
	{
		TArray<FString> Inputs = FJungleTerrainSourceSpec::RequiredLayerNames();
		Inputs.Append(FJungleEcosystemMaskSpec::RequiredMaskNames());
		return Inputs;
	}

	static TArray<FString> PlaceholderOutputNames()
	{
		return {
			TEXT("pcg_canopy_placeholder"),
			TEXT("pcg_understory_placeholder"),
			TEXT("pcg_ground_clutter_placeholder"),
			TEXT("pcg_deadfall_placeholder"),
			TEXT("pcg_rock_root_placeholder"),
			TEXT("pcg_plant_friction_placeholder"),
			TEXT("pcg_affordance_cue_placeholder"),
			TEXT("pcg_false_affordance_cue_placeholder"),
			TEXT("pcg_sound_anchor_placeholder"),
			TEXT("pcg_sky_window_preservation_zone"),
			TEXT("pcg_landmark_silhouette_placeholder"),
		};
	}

	static TArray<FString> RequiredDebugOutputNames()
	{
		TArray<FString> Outputs = FJungleEcosystemDebugSpec::RequiredValidationViews();
		Outputs.Append({
			TEXT("pcg_biome_layer"),
			TEXT("pcg_spawn_rule_budget"),
			TEXT("pcg_terrain_friction"),
			TEXT("pcg_region_seed"),
			TEXT("pcg_streaming_region_scope"),
		});
		return Outputs;
	}
};
