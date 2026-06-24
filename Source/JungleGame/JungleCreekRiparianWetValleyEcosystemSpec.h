#pragma once

#include "CoreMinimal.h"
#include "JungleEcosystemMaskSpec.h"
#include "JungleMacroTerrainSpec.h"
#include "JunglePCGBiomeSpec.h"

/** Creek and wet-valley subzones consumed by later PCG graph assets. */
enum class EJungleCreekRiparianWetValleyZone : uint8
{
	OutsideCreekInfluence,
	CreekChannelHardWater,
	CreekBankAffordance,
	CreekCrossingCandidate,
	RiparianBench,
	WetValleyFloor,
	MuddyDeadfallSoftBlocker,
	WaitAWhileSoftBlocker,
	WaterSoundAnchor,
	FalseCreekAffordance,
};

/** Placeholder creek evidence types; these are contracts, not final water, audio, or wildlife systems. */
enum class EJungleCreekEvidenceCue : uint8
{
	ChannelSound,
	BankVegetationChange,
	WetRockRoot,
	MudShelf,
	DeadfallTangle,
	CrossingShallowShelf,
	LeechTickRiskNote,
	FrogInsectSoundNote,
	MuffledRainWaterNote,
	FalseBankOpening,
};

/** Reviewable contract for a future creek/riparian PCG rule. */
struct FJungleCreekRiparianRuleContract
{
	EJungleCreekRiparianWetValleyZone Zone = EJungleCreekRiparianWetValleyZone::OutsideCreekInfluence;
	EJungleCreekEvidenceCue EvidenceCue = EJungleCreekEvidenceCue::BankVegetationChange;
	EJunglePCGSpawnRuleKind RuleKind = EJunglePCGSpawnRuleKind::GroundClutterPlaceholder;
	EJunglePCGTerrainFrictionClass FrictionClass = EJunglePCGTerrainFrictionClass::Open;
	TArray<FString> RequiredMaskNames;
	float MaxInstancesPerHectare = 0.0f;
	float MinimumSpacingMeters = 1.0f;
	bool bCanMarkCrossingCandidate = false;
	bool bCanCreateFalseAffordance = false;
	bool bDeveloperOnly = true;

	bool IsReviewable() const
	{
		return MaxInstancesPerHectare >= 0.0f
			&& MinimumSpacingMeters > 0.0f
			&& RequiredMaskNames.Num() > 0;
	}
};

struct FJungleCreekRiparianWetValleyEcosystemSpec
{
	static constexpr int32 EcosystemPassVersion = 1;
	static constexpr float ChannelEdgeMeters = 8.0f;
	static constexpr float CreekBankAffordanceMeters = 24.0f;
	static constexpr float RiparianBenchMeters = 48.0f;
	static constexpr float WetValleyInfluenceMeters = FJunglePCGBiomeFrameworkSpec::CreekInfluenceMeters;
	static constexpr float MaxCrossingSlopeDegrees = 18.0f;
	static constexpr float MaxWetValleyFloorSlopeDegrees = 12.0f;
	static constexpr float MinimumCrossingAffordance = 0.72f;
	static constexpr float MaximumCrossingFootingRisk = 0.48f;
	static constexpr float MuddySoftBlockerWetness = 0.74f;
	static constexpr float WaitAWhileSoftBlockerCanopy = 0.68f;
	static constexpr float MinimumSoundAnchorPropagation = 0.62f;

	static bool AcceptsFinalWaterSimulation()
	{
		return false;
	}

	static bool AcceptsFinalAudioMix()
	{
		return false;
	}

	static bool AcceptsFullWildlifeAI()
	{
		return false;
	}

	static bool IsInsideCreekInfluence(const FJunglePCGBiomeInputSample& Sample)
	{
		return Sample.CreekDistanceMeters <= WetValleyInfluenceMeters
			|| Sample.GetMaskValue(TEXT("wetness")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold;
	}

	static bool IsPotentialCrossingCandidate(const FJunglePCGBiomeInputSample& Sample)
	{
		return Sample.CreekDistanceMeters <= CreekBankAffordanceMeters
			&& Sample.SlopeDegrees <= MaxCrossingSlopeDegrees
			&& Sample.GetMaskValue(TEXT("affordance_zone")) >= MinimumCrossingAffordance
			&& Sample.GetMaskValue(TEXT("footing_risk")) <= MaximumCrossingFootingRisk
			&& Sample.GetMaskValue(TEXT("hard_blocker")) < FJungleEcosystemMaskSpec::HardBlockerThreshold;
	}

	static EJungleCreekRiparianWetValleyZone ClassifyCreekZone(const FJunglePCGBiomeInputSample& Sample)
	{
		if (!IsInsideCreekInfluence(Sample))
		{
			return EJungleCreekRiparianWetValleyZone::OutsideCreekInfluence;
		}

		if (Sample.GetMaskValue(TEXT("false_affordance")) >= FJungleEcosystemMaskSpec::FalseAffordanceThreshold)
		{
			return EJungleCreekRiparianWetValleyZone::FalseCreekAffordance;
		}

		if (Sample.GetMaskValue(TEXT("hard_blocker")) >= FJungleEcosystemMaskSpec::HardBlockerThreshold
			&& Sample.CreekDistanceMeters <= ChannelEdgeMeters)
		{
			return EJungleCreekRiparianWetValleyZone::CreekChannelHardWater;
		}

		if (IsPotentialCrossingCandidate(Sample))
		{
			return EJungleCreekRiparianWetValleyZone::CreekCrossingCandidate;
		}

		if (Sample.GetMaskValue(TEXT("soft_blocker")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold
			&& Sample.GetMaskValue(TEXT("canopy")) >= WaitAWhileSoftBlockerCanopy)
		{
			return EJungleCreekRiparianWetValleyZone::WaitAWhileSoftBlocker;
		}

		if (Sample.GetMaskValue(TEXT("wetness")) >= MuddySoftBlockerWetness
			|| Sample.GetMaskValue(TEXT("footing_risk")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold)
		{
			return EJungleCreekRiparianWetValleyZone::MuddyDeadfallSoftBlocker;
		}

		if (Sample.GetMaskValue(TEXT("sound_propagation")) >= MinimumSoundAnchorPropagation
			&& Sample.CreekDistanceMeters <= RiparianBenchMeters)
		{
			return EJungleCreekRiparianWetValleyZone::WaterSoundAnchor;
		}

		if (Sample.SlopeDegrees <= MaxWetValleyFloorSlopeDegrees
			&& Sample.GetMaskValue(TEXT("wetness")) >= FJungleEcosystemMaskSpec::AffordanceThreshold)
		{
			return EJungleCreekRiparianWetValleyZone::WetValleyFloor;
		}

		if (Sample.CreekDistanceMeters <= CreekBankAffordanceMeters)
		{
			return EJungleCreekRiparianWetValleyZone::CreekBankAffordance;
		}

		return EJungleCreekRiparianWetValleyZone::RiparianBench;
	}

	static EJunglePCGTerrainFrictionClass ResolveCreekFriction(const FJunglePCGBiomeInputSample& Sample)
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
			TEXT("creek_distance"),
			TEXT("wetness"),
			TEXT("slope"),
			TEXT("canopy"),
			TEXT("footing_risk"),
			TEXT("sound_propagation"),
			TEXT("visibility"),
			TEXT("hard_blocker"),
			TEXT("soft_blocker"),
			TEXT("affordance_zone"),
			TEXT("false_affordance"),
		};
	}

	static TArray<FJungleCreekRiparianRuleContract> PlaceholderRuleContracts()
	{
		TArray<FJungleCreekRiparianRuleContract> Rules;

		FJungleCreekRiparianRuleContract BankAffordance;
		BankAffordance.Zone = EJungleCreekRiparianWetValleyZone::CreekBankAffordance;
		BankAffordance.EvidenceCue = EJungleCreekEvidenceCue::BankVegetationChange;
		BankAffordance.RuleKind = EJunglePCGSpawnRuleKind::AffordanceCuePlaceholder;
		BankAffordance.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		BankAffordance.RequiredMaskNames = { TEXT("creek_distance"), TEXT("affordance_zone"), TEXT("visibility") };
		BankAffordance.MaxInstancesPerHectare = 36.0f;
		BankAffordance.MinimumSpacingMeters = 6.0f;
		Rules.Add(BankAffordance);

		FJungleCreekRiparianRuleContract CrossingCandidate;
		CrossingCandidate.Zone = EJungleCreekRiparianWetValleyZone::CreekCrossingCandidate;
		CrossingCandidate.EvidenceCue = EJungleCreekEvidenceCue::CrossingShallowShelf;
		CrossingCandidate.RuleKind = EJunglePCGSpawnRuleKind::AffordanceCuePlaceholder;
		CrossingCandidate.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		CrossingCandidate.RequiredMaskNames = { TEXT("creek_distance"), TEXT("slope"), TEXT("footing_risk"), TEXT("affordance_zone") };
		CrossingCandidate.MaxInstancesPerHectare = 8.0f;
		CrossingCandidate.MinimumSpacingMeters = 18.0f;
		CrossingCandidate.bCanMarkCrossingCandidate = true;
		Rules.Add(CrossingCandidate);

		FJungleCreekRiparianRuleContract MuddyDeadfall;
		MuddyDeadfall.Zone = EJungleCreekRiparianWetValleyZone::MuddyDeadfallSoftBlocker;
		MuddyDeadfall.EvidenceCue = EJungleCreekEvidenceCue::DeadfallTangle;
		MuddyDeadfall.RuleKind = EJunglePCGSpawnRuleKind::DeadfallPlaceholder;
		MuddyDeadfall.FrictionClass = EJunglePCGTerrainFrictionClass::ReadablePlantFriction;
		MuddyDeadfall.RequiredMaskNames = { TEXT("wetness"), TEXT("soft_blocker"), TEXT("footing_risk") };
		MuddyDeadfall.MaxInstancesPerHectare = 52.0f;
		MuddyDeadfall.MinimumSpacingMeters = 4.0f;
		Rules.Add(MuddyDeadfall);

		FJungleCreekRiparianRuleContract SoundAnchor;
		SoundAnchor.Zone = EJungleCreekRiparianWetValleyZone::WaterSoundAnchor;
		SoundAnchor.EvidenceCue = EJungleCreekEvidenceCue::ChannelSound;
		SoundAnchor.RuleKind = EJunglePCGSpawnRuleKind::SoundAnchorPlaceholder;
		SoundAnchor.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		SoundAnchor.RequiredMaskNames = { TEXT("creek_distance"), TEXT("sound_propagation"), TEXT("visibility") };
		SoundAnchor.MaxInstancesPerHectare = 12.0f;
		SoundAnchor.MinimumSpacingMeters = 12.0f;
		Rules.Add(SoundAnchor);

		FJungleCreekRiparianRuleContract FalseOpening;
		FalseOpening.Zone = EJungleCreekRiparianWetValleyZone::FalseCreekAffordance;
		FalseOpening.EvidenceCue = EJungleCreekEvidenceCue::FalseBankOpening;
		FalseOpening.RuleKind = EJunglePCGSpawnRuleKind::FalseAffordanceCuePlaceholder;
		FalseOpening.FrictionClass = EJunglePCGTerrainFrictionClass::FalseAffordance;
		FalseOpening.RequiredMaskNames = { TEXT("false_affordance"), TEXT("creek_distance"), TEXT("soft_blocker") };
		FalseOpening.MaxInstancesPerHectare = 10.0f;
		FalseOpening.MinimumSpacingMeters = 14.0f;
		FalseOpening.bCanCreateFalseAffordance = true;
		Rules.Add(FalseOpening);

		return Rules;
	}

	static TArray<FString> RequiredValidationOutputNames()
	{
		return {
			TEXT("creek_riparian_zone"),
			TEXT("creek_crossing_candidate"),
			TEXT("creek_bank_affordance"),
			TEXT("wet_valley_soft_blocker"),
			TEXT("water_sound_anchor"),
			TEXT("creek_false_affordance"),
			TEXT("leech_tick_placeholder_risk_note"),
		};
	}
};
