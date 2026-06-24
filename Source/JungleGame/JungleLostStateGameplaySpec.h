#pragma once

#include "CoreMinimal.h"
#include "JungleNavigationPillarsSpec.h"

/** Internal lost-state pressure phases. These are not player-facing UI labels. */
enum class EJungleLostStatePressurePhase : uint8
{
	GroundedByWorldEvidence,
	QuestioningRoute,
	LostPressure,
	RecoveryEvidenceAvailable,
	CriticalExposurePressure,
};

/** Environmental evidence that can push the player toward or away from lost-state pressure. */
enum class EJungleLostStateEvidenceSource : uint8
{
	DistanceFromKnownSafety,
	ReadableCueLoss,
	DarknessPressure,
	RainMuffledSound,
	DenseCanopyOcclusion,
	GorgeCompression,
	SteepSlopePressure,
	HardBlockerPressure,
	FalseAffordancePressure,
	TrailMarkDegradation,
	KnownSafetyAnchor,
	RecoveryCue,
};

/** Diegetic recovery evidence types. These are not objective arrows or route lines. */
enum class EJungleLostRecoveryCue : uint8
{
	PlayerMadeTrailMark,
	CreekFlowEvidence,
	CoastOceanSound,
	RidgeSkyWindow,
	MountainSilhouette,
	SlopeFallLine,
	SouthernSkyWindow,
	KnownSafetyLineOfSight,
};

/** Coarse cue snapshot produced by developer-only validation or future gameplay code. */
struct FJungleLostStateCueSnapshot
{
	int32 ReadableCueCount = 0;
	int32 DegradedCueCount = 0;
	int32 UnavailableCueCount = 0;

	bool HasReadableWorldEvidence() const
	{
		return ReadableCueCount > 0;
	}

	bool HasMostlyUnavailableEvidence() const
	{
		return UnavailableCueCount > ReadableCueCount + DegradedCueCount;
	}
};

/** Input sample for lost-state pressure. Values are internal gameplay/debug data, not player UI. */
struct FJungleLostStateInputSample
{
	FJungleNavigationCueSample NavigationSample;
	FJungleLostStateCueSnapshot CueSnapshot;
	float DistanceFromKnownSafetyMeters = 0.0f;
	float TrailMarkVisibility = 0.0f;
	float TrailMarkIntegrity = 0.0f;
	float KnownSafetyAnchorSignal = 0.0f;
	float RecoveryCueSignal = 0.0f;
	bool bDarknessPressure = false;
	bool bRainMuffledSound = false;
	bool bKnownSafetyLineOfSight = false;
	bool bFalseAffordancePressure = false;
};

/** Contract for one lost-state pressure or recovery rule. */
struct FJungleLostStateRuleContract
{
	EJungleLostStateEvidenceSource EvidenceSource = EJungleLostStateEvidenceSource::DistanceFromKnownSafety;
	TArray<FString> RequiredMaskNames;
	bool bCanEscalatePressure = false;
	bool bCanProvideRecoveryEvidence = false;
	bool bPlayerFacingDiegetic = true;
	bool bDeveloperOnlyDebugOutput = false;
	bool bAllowsMapLikeUI = false;
	bool bAllowsCompassOrBearing = false;
	bool bAllowsClockOrExplicitTime = false;
	bool bAllowsObjectiveMarker = false;
	bool bAllowsCharacterLostCommentary = false;
	bool bAllowsOrientationConfidenceFeedback = false;

	bool IsWorldOnly() const
	{
		return !bAllowsMapLikeUI
			&& !bAllowsCompassOrBearing
			&& !bAllowsClockOrExplicitTime
			&& !bAllowsObjectiveMarker
			&& !bAllowsCharacterLostCommentary
			&& !bAllowsOrientationConfidenceFeedback;
	}

	bool IsReviewable() const
	{
		return IsWorldOnly()
			&& (bPlayerFacingDiegetic || bDeveloperOnlyDebugOutput)
			&& RequiredMaskNames.Num() > 0
			&& (bCanEscalatePressure || bCanProvideRecoveryEvidence);
	}
};

struct FJungleLostStateGameplaySpec
{
	static constexpr int32 LostStateGameplayVersion = 1;
	static constexpr float QuestioningRouteThreshold = 0.38f;
	static constexpr float LostPressureThreshold = 0.62f;
	static constexpr float CriticalExposureThreshold = 0.82f;
	static constexpr float RecoveryEvidenceThreshold = 0.58f;
	static constexpr float KnownSafetyNearMeters = 128.0f;
	static constexpr float KnownSafetyFarMeters = 1024.0f;
	static constexpr float CueLossWeight = 0.30f;
	static constexpr float DistanceWeight = 0.34f;
	static constexpr float TerrainPressureWeight = 0.28f;
	static constexpr float WeatherDarknessWeight = 0.16f;
	static constexpr float RecoveryReliefWeight = 0.35f;

	static bool AcceptsPlayerMap()
	{
		return false;
	}

	static bool AcceptsCompassOrBearingReadout()
	{
		return false;
	}

	static bool AcceptsGPSOrObjectiveMarkerNavigation()
	{
		return false;
	}

	static bool AcceptsClockOrExplicitTimeReadout()
	{
		return false;
	}

	static bool AcceptsOrientationConfidenceFeedback()
	{
		return false;
	}

	static bool AcceptsCharacterLostCommentary()
	{
		return false;
	}

	static bool AcceptsFullSurvivalStatStack()
	{
		return false;
	}

	static bool AcceptsPlayerMadeTrailMarksAsPCGGenerationInput()
	{
		return false;
	}

	static bool AllowsPlayerMadeTrailMarkInteractions()
	{
		return true;
	}

	static float ClampPressure(float Value)
	{
		return FMath::Clamp(Value, FJungleEcosystemMaskSpec::NormalizedMin, FJungleEcosystemMaskSpec::NormalizedMax);
	}

	static float ResolveDistancePressure(float DistanceFromKnownSafetyMeters)
	{
		if (DistanceFromKnownSafetyMeters <= KnownSafetyNearMeters)
		{
			return 0.0f;
		}

		return ClampPressure((DistanceFromKnownSafetyMeters - KnownSafetyNearMeters) / (KnownSafetyFarMeters - KnownSafetyNearMeters));
	}

	static float ResolveCueLossPressure(const FJungleLostStateCueSnapshot& Snapshot)
	{
		const float UnavailablePressure = static_cast<float>(Snapshot.UnavailableCueCount) * 0.16f;
		const float DegradedPressure = static_cast<float>(Snapshot.DegradedCueCount) * 0.07f;
		const float ReadableRelief = static_cast<float>(Snapshot.ReadableCueCount) * 0.12f;
		return ClampPressure(UnavailablePressure + DegradedPressure - ReadableRelief);
	}

	static float ResolveTerrainPressure(const FJunglePCGBiomeInputSample& Sample)
	{
		const float BlockerPressure = FMath::Max3(
			Sample.GetMaskValue(TEXT("hard_blocker")),
			Sample.GetMaskValue(TEXT("soft_blocker")),
			Sample.GetMaskValue(TEXT("false_affordance")));
		const float FootingPressure = Sample.GetMaskValue(TEXT("footing_risk"));
		const float NightPressure = Sample.GetMaskValue(TEXT("night_danger"));
		const float SlopePressure = ClampPressure(Sample.SlopeDegrees / FJungleMacroTerrainSpec::HardBlockerSlopeDegrees);
		return ClampPressure(FMath::Max(FMath::Max3(BlockerPressure, FootingPressure, NightPressure), SlopePressure));
	}

	static float ResolveRecoveryEvidenceSignal(const FJungleLostStateInputSample& Sample)
	{
		const float TrailMarkSignal = FMath::Max(Sample.TrailMarkVisibility, Sample.TrailMarkIntegrity);
		const float KnownSafetySignal = Sample.bKnownSafetyLineOfSight
			? FMath::Max(Sample.KnownSafetyAnchorSignal, RecoveryEvidenceThreshold)
			: Sample.KnownSafetyAnchorSignal;
		return ClampPressure(FMath::Max3(TrailMarkSignal, KnownSafetySignal, Sample.RecoveryCueSignal));
	}

	static float ResolveLostStatePressureScore(const FJungleLostStateInputSample& Sample)
	{
		float Score = 0.0f;
		Score += ResolveDistancePressure(Sample.DistanceFromKnownSafetyMeters) * DistanceWeight;
		Score += ResolveCueLossPressure(Sample.CueSnapshot) * CueLossWeight;
		Score += ResolveTerrainPressure(Sample.NavigationSample.TerrainSample) * TerrainPressureWeight;

		if (Sample.bDarknessPressure)
		{
			Score += WeatherDarknessWeight;
		}

		if (Sample.bRainMuffledSound)
		{
			Score += WeatherDarknessWeight * 0.5f;
		}

		if (Sample.bFalseAffordancePressure)
		{
			Score += TerrainPressureWeight * 0.5f;
		}

		Score -= ResolveRecoveryEvidenceSignal(Sample) * RecoveryReliefWeight;
		return ClampPressure(Score);
	}

	static EJungleLostStatePressurePhase ResolvePressurePhase(const FJungleLostStateInputSample& Sample)
	{
		const float PressureScore = ResolveLostStatePressureScore(Sample);
		const float RecoverySignal = ResolveRecoveryEvidenceSignal(Sample);

		if (PressureScore >= CriticalExposureThreshold)
		{
			return EJungleLostStatePressurePhase::CriticalExposurePressure;
		}

		if (RecoverySignal >= RecoveryEvidenceThreshold && PressureScore >= QuestioningRouteThreshold)
		{
			return EJungleLostStatePressurePhase::RecoveryEvidenceAvailable;
		}

		if (PressureScore >= LostPressureThreshold)
		{
			return EJungleLostStatePressurePhase::LostPressure;
		}

		if (PressureScore >= QuestioningRouteThreshold)
		{
			return EJungleLostStatePressurePhase::QuestioningRoute;
		}

		return EJungleLostStatePressurePhase::GroundedByWorldEvidence;
	}

	static bool HasRecoveryEvidence(const FJungleLostStateInputSample& Sample)
	{
		return ResolveRecoveryEvidenceSignal(Sample) >= RecoveryEvidenceThreshold
			|| Sample.CueSnapshot.HasReadableWorldEvidence();
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
			TEXT("sky_window"),
			TEXT("footing_risk"),
			TEXT("night_danger"),
			TEXT("hard_blocker"),
			TEXT("soft_blocker"),
			TEXT("affordance_zone"),
			TEXT("false_affordance"),
		};
	}

	static TArray<FJungleLostStateRuleContract> RuleContracts()
	{
		TArray<FJungleLostStateRuleContract> Rules;

		FJungleLostStateRuleContract Distance;
		Distance.EvidenceSource = EJungleLostStateEvidenceSource::DistanceFromKnownSafety;
		Distance.RequiredMaskNames = { TEXT("visibility"), TEXT("sound_propagation"), TEXT("disturbance") };
		Distance.bCanEscalatePressure = true;
		Rules.Add(Distance);

		FJungleLostStateRuleContract CueLoss;
		CueLoss.EvidenceSource = EJungleLostStateEvidenceSource::ReadableCueLoss;
		CueLoss.RequiredMaskNames = { TEXT("sky_window"), TEXT("visibility"), TEXT("canopy"), TEXT("sound_propagation") };
		CueLoss.bCanEscalatePressure = true;
		Rules.Add(CueLoss);

		FJungleLostStateRuleContract Darkness;
		Darkness.EvidenceSource = EJungleLostStateEvidenceSource::DarknessPressure;
		Darkness.RequiredMaskNames = { TEXT("night_danger"), TEXT("sky_window"), TEXT("visibility") };
		Darkness.bCanEscalatePressure = true;
		Rules.Add(Darkness);

		FJungleLostStateRuleContract RainMuffle;
		RainMuffle.EvidenceSource = EJungleLostStateEvidenceSource::RainMuffledSound;
		RainMuffle.RequiredMaskNames = { TEXT("sound_propagation"), TEXT("creek_distance"), TEXT("coast_distance"), TEXT("visibility") };
		RainMuffle.bCanEscalatePressure = true;
		Rules.Add(RainMuffle);

		FJungleLostStateRuleContract GorgeCompression;
		GorgeCompression.EvidenceSource = EJungleLostStateEvidenceSource::GorgeCompression;
		GorgeCompression.RequiredMaskNames = { TEXT("ridge_valley"), TEXT("visibility"), TEXT("sound_propagation"), TEXT("hard_blocker") };
		GorgeCompression.bCanEscalatePressure = true;
		Rules.Add(GorgeCompression);

		FJungleLostStateRuleContract SteepSlope;
		SteepSlope.EvidenceSource = EJungleLostStateEvidenceSource::SteepSlopePressure;
		SteepSlope.RequiredMaskNames = { TEXT("slope"), TEXT("footing_risk"), TEXT("soft_blocker"), TEXT("affordance_zone") };
		SteepSlope.bCanEscalatePressure = true;
		Rules.Add(SteepSlope);

		FJungleLostStateRuleContract FalseAffordance;
		FalseAffordance.EvidenceSource = EJungleLostStateEvidenceSource::FalseAffordancePressure;
		FalseAffordance.RequiredMaskNames = { TEXT("false_affordance"), TEXT("visibility"), TEXT("soft_blocker"), TEXT("footing_risk") };
		FalseAffordance.bCanEscalatePressure = true;
		Rules.Add(FalseAffordance);

		FJungleLostStateRuleContract TrailMark;
		TrailMark.EvidenceSource = EJungleLostStateEvidenceSource::TrailMarkDegradation;
		TrailMark.RequiredMaskNames = { TEXT("disturbance"), TEXT("visibility"), TEXT("wetness"), TEXT("canopy") };
		TrailMark.bCanEscalatePressure = true;
		TrailMark.bCanProvideRecoveryEvidence = true;
		Rules.Add(TrailMark);

		FJungleLostStateRuleContract KnownSafety;
		KnownSafety.EvidenceSource = EJungleLostStateEvidenceSource::KnownSafetyAnchor;
		KnownSafety.RequiredMaskNames = { TEXT("visibility"), TEXT("sound_propagation"), TEXT("sky_window") };
		KnownSafety.bCanProvideRecoveryEvidence = true;
		Rules.Add(KnownSafety);

		FJungleLostStateRuleContract RecoveryCue;
		RecoveryCue.EvidenceSource = EJungleLostStateEvidenceSource::RecoveryCue;
		RecoveryCue.RequiredMaskNames = { TEXT("creek_distance"), TEXT("coast_distance"), TEXT("ridge_valley"), TEXT("sky_window"), TEXT("sound_propagation"), TEXT("affordance_zone") };
		RecoveryCue.bCanProvideRecoveryEvidence = true;
		Rules.Add(RecoveryCue);

		return Rules;
	}

	static TArray<FString> RequiredDebugOutputNames()
	{
		return {
			TEXT("lost_state_rule_contracts"),
			TEXT("lost_state_distance_pressure"),
			TEXT("lost_state_cue_loss_pressure"),
			TEXT("lost_state_terrain_pressure"),
			TEXT("lost_state_recovery_evidence"),
			TEXT("lost_state_trail_mark_runtime_interaction"),
			TEXT("lost_state_forbidden_ui_absence_check"),
		};
	}
};


codex
No discrete, commit-introduced correctness issues were found in the PR17 lost-state scaffold. The changes are source/docs only, and I did not find a specific bug requiring a code change.
No discrete, commit-introduced correctness issues were found in the PR17 lost-state scaffold. The changes are source/docs only, and I did not find a specific bug requiring a code change.

[Showing lines 1433-2765 of 2765 (50.0KB limit). Full output: /tmp/pi-bash-51b361d41da54d7d.log]