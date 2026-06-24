#pragma once

#include "CoreMinimal.h"
#include "JungleCreekRiparianWetValleyEcosystemSpec.h"
#include "JungleLowlandAlluvialGorgeEcosystemSpec.h"
#include "JungleRidgeSlopeMountainEcosystemSpec.h"

/** Diegetic navigation evidence pillars. These are not UI navigation aids. */
enum class EJungleNavigationCuePillar : uint8
{
	SunPosition,
	ShadowDirectionLength,
	LightColour,
	CreekFlow,
	SlopeGradient,
	CoastOcean,
	RidgeSpurSaddle,
	MountainLandmark,
	SkyWindow,
	SouthernSkyStarsMoon,
	EnvironmentalSound,
	TraversalAffordance,
	PlayerMadeTrailMark,
};

/** How a cue reaches the player through the world. */
enum class EJungleNavigationCueModality : uint8
{
	VisualLight,
	VisualLandform,
	AuditoryEnvironment,
	TraversalFriction,
	RuntimePlayerMark,
};

/** Environmental reasons a cue may become imperfect or unavailable. */
enum class EJungleNavigationCueDegrader : uint8
{
	DenseCanopy,
	CloudCover,
	RainMuffle,
	GorgeOcclusion,
	NightNoSun,
	NoSkyWindow,
	LineOfSightBlocked,
};

/** Coarse developer-readability state for one environmental cue. Not player-facing feedback. */
enum class EJungleNavigationCueReadability : uint8
{
	Unavailable,
	Degraded,
	Readable,
};

/** Transient weather/visibility hooks used by the cue contracts. Not a final weather simulation. */
struct FJungleNavigationEnvironmentalState
{
	bool bDaylight = true;
	bool bNight = false;
	bool bClearSky = true;
	bool bDenseCanopy = false;
	bool bCloudCover = false;
	bool bRain = false;
	bool bInsideGorge = false;
	bool bSkyWindowVisible = false;
	bool bMoonVisible = false;
	bool bSouthernSkyVisible = false;
	bool bOceanAudible = false;
	bool bLandmarkLineOfSight = false;
};

/** Reviewable cue contract for future actor/component/material/audio hooks. */
struct FJungleNavigationCueContract
{
	EJungleNavigationCuePillar Pillar = EJungleNavigationCuePillar::SunPosition;
	EJungleNavigationCueModality Modality = EJungleNavigationCueModality::VisualLight;
	TArray<FString> RequiredMaskNames;
	TArray<EJungleNavigationCueDegrader> ExpectedDegraders;
	bool bPlayerFacingDiegetic = true;
	bool bDeveloperOnlyDebugOutput = false;
	bool bRequiresRuntimePersistence = false;
	bool bAllowsMapLikeUI = false;
	bool bAllowsBearingReadout = false;
	bool bAllowsClockReadout = false;
	bool bAllowsObjectiveMarker = false;
	bool bAllowsOrientationConfidenceFeedback = false;

	bool IsWorldEvidenceOnly() const
	{
		return !bAllowsMapLikeUI
			&& !bAllowsBearingReadout
			&& !bAllowsClockReadout
			&& !bAllowsObjectiveMarker
			&& !bAllowsOrientationConfidenceFeedback;
	}

	bool IsReviewable() const
	{
		return IsWorldEvidenceOnly()
			&& (bPlayerFacingDiegetic || bDeveloperOnlyDebugOutput)
			&& (RequiredMaskNames.Num() > 0 || bRequiresRuntimePersistence);
	}
};

/** Sample state consumed by navigation cue helpers. World vectors are contract/debug data, not player readouts. */
struct FJungleNavigationCueSample
{
	FJunglePCGBiomeInputSample TerrainSample;
	FJungleNavigationEnvironmentalState Environment;
	FVector2D CreekFlowWorldDirection = FVector2D::ZeroVector;
	FVector2D SlopeFallLineWorldDirection = FVector2D::ZeroVector;
	float NormalizedSunVisibility = 0.0f;
	float ShadowLegibility = 0.0f;
	float LightColourWarmth = 0.0f;
	float LandmarkContrast = 0.0f;
	float OceanSoundAudibility = 0.0f;
	float StarMoonVisibility = 0.0f;
	bool bHasCreekFlowDirection = false;
	bool bHasSlopeFallLine = false;
};

struct FJungleNavigationPillarsSpec
{
	static constexpr int32 NavigationPillarsVersion = 1;
	static constexpr float MinimumReadableCueSignal = 0.62f;
	static constexpr float MinimumDegradedCueSignal = 0.34f;
	static constexpr float MinimumSunSkyWindow = 0.38f;
	static constexpr float MinimumShadowLegibility = 0.45f;
	static constexpr float MinimumLightColourCue = 0.40f;
	static constexpr float MinimumReadableSlopeDegrees = 6.0f;
	static constexpr float MinimumAudibleNavigationSignal = 0.50f;
	static constexpr float MinimumLandmarkContrast = 0.55f;
	static constexpr float MinimumSouthernSkyVisibility = 0.42f;

	static bool AcceptsPlayerMap()
	{
		return false;
	}

	static bool AcceptsMinimap()
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

	static bool AcceptsFinalWeatherSimulation()
	{
		return false;
	}

	static bool AcceptsFinalAudioMix()
	{
		return false;
	}

	static bool AcceptsFinalSkyStarArt()
	{
		return false;
	}

	static bool AcceptsRuntimePlayerTrailMarksAsPCGGenerationInput()
	{
		return false;
	}

	static bool AllowsRuntimePlayerTrailMarkPersistence()
	{
		return true;
	}

	static float ClampCueSignal(float Value)
	{
		return FMath::Clamp(Value, FJungleEcosystemMaskSpec::NormalizedMin, FJungleEcosystemMaskSpec::NormalizedMax);
	}

	static float EnvironmentalVisibilityMultiplier(const FJungleNavigationEnvironmentalState& Environment)
	{
		float Multiplier = 1.0f;

		if (Environment.bDenseCanopy)
		{
			Multiplier *= 0.55f;
		}

		if (Environment.bCloudCover)
		{
			Multiplier *= 0.70f;
		}

		if (Environment.bRain)
		{
			Multiplier *= 0.72f;
		}

		if (Environment.bInsideGorge)
		{
			Multiplier *= 0.62f;
		}

		return ClampCueSignal(Multiplier);
	}

	static EJungleNavigationCueReadability ResolveCueSignal(float Signal)
	{
		const float ClampedSignal = ClampCueSignal(Signal);

		if (ClampedSignal >= MinimumReadableCueSignal)
		{
			return EJungleNavigationCueReadability::Readable;
		}

		if (ClampedSignal >= MinimumDegradedCueSignal)
		{
			return EJungleNavigationCueReadability::Degraded;
		}

		return EJungleNavigationCueReadability::Unavailable;
	}

	static EJungleNavigationCueReadability ResolveReadability(float RawSignal, const FJungleNavigationEnvironmentalState& Environment)
	{
		return ResolveCueSignal(ClampCueSignal(RawSignal) * EnvironmentalVisibilityMultiplier(Environment));
	}

	static float EnvironmentalAudioMultiplier(const FJungleNavigationEnvironmentalState& Environment)
	{
		float Multiplier = 1.0f;

		if (Environment.bRain)
		{
			Multiplier *= 0.68f;
		}

		if (Environment.bInsideGorge)
		{
			Multiplier *= 0.76f;
		}

		return ClampCueSignal(Multiplier);
	}

	static EJungleNavigationCueReadability ResolveAuditoryReadability(float RawSignal, const FJungleNavigationEnvironmentalState& Environment)
	{
		return ResolveCueSignal(ClampCueSignal(RawSignal) * EnvironmentalAudioMultiplier(Environment));
	}

	static EJungleNavigationCueReadability ResolveSunPositionCue(const FJungleNavigationCueSample& Sample)
	{
		if (!Sample.Environment.bDaylight || Sample.Environment.bNight)
		{
			return EJungleNavigationCueReadability::Unavailable;
		}

		const float SkyWindowSignal = Sample.TerrainSample.GetMaskValue(TEXT("sky_window")) * Sample.TerrainSample.GetMaskValue(TEXT("visibility"));
		return ResolveReadability(FMath::Max(Sample.NormalizedSunVisibility, SkyWindowSignal), Sample.Environment);
	}

	static EJungleNavigationCueReadability ResolveShadowCue(const FJungleNavigationCueSample& Sample)
	{
		if (!Sample.Environment.bDaylight || Sample.Environment.bNight)
		{
			return EJungleNavigationCueReadability::Unavailable;
		}

		return ResolveReadability(Sample.ShadowLegibility, Sample.Environment);
	}

	static EJungleNavigationCueReadability ResolveLightColourCue(const FJungleNavigationCueSample& Sample)
	{
		if (!Sample.Environment.bDaylight || Sample.Environment.bNight)
		{
			return EJungleNavigationCueReadability::Unavailable;
		}

		return ResolveReadability(Sample.LightColourWarmth, Sample.Environment);
	}

	static EJungleNavigationCueReadability ResolveCreekFlowCue(const FJungleNavigationCueSample& Sample)
	{
		if (!Sample.bHasCreekFlowDirection
			|| !FJungleCreekRiparianWetValleyEcosystemSpec::IsInsideCreekInfluence(Sample.TerrainSample))
		{
			return EJungleNavigationCueReadability::Unavailable;
		}

		const float CreekSignal = FMath::Max(
			Sample.TerrainSample.GetMaskValue(TEXT("wetness")),
			Sample.TerrainSample.GetMaskValue(TEXT("sound_propagation")));
		return ResolveReadability(CreekSignal, Sample.Environment);
	}

	static EJungleNavigationCueReadability ResolveSlopeCue(const FJungleNavigationCueSample& Sample)
	{
		if (!Sample.bHasSlopeFallLine || FJungleMacroTerrainSpec::IsSlopeHardBlocker(Sample.TerrainSample.SlopeDegrees))
		{
			return EJungleNavigationCueReadability::Unavailable;
		}

		const float SlopeSignal = Sample.TerrainSample.SlopeDegrees >= MinimumReadableSlopeDegrees
			? ClampCueSignal(Sample.TerrainSample.SlopeDegrees / FJungleMacroTerrainSpec::MaximumRoutineWalkableSlopeDegrees)
			: 0.0f;
		return ResolveReadability(SlopeSignal, Sample.Environment);
	}

	static EJungleNavigationCueReadability ResolveCoastOceanCue(const FJungleNavigationCueSample& Sample)
	{
		const bool bInsideCoastalTransition = Sample.TerrainSample.CoastDistanceMeters <= FJunglePCGBiomeFrameworkSpec::CoastEdgeMeters;
		const float CoastDistanceSignal = bInsideCoastalTransition ? 1.0f : 0.0f;
		const float OceanSoundSignal = Sample.Environment.bOceanAudible ? FMath::Max(Sample.OceanSoundAudibility, MinimumAudibleNavigationSignal) : Sample.OceanSoundAudibility;
		return ResolveAuditoryReadability(FMath::Max(CoastDistanceSignal, OceanSoundSignal), Sample.Environment);
	}

	static EJungleNavigationCueReadability ResolveRidgeLandmarkCue(const FJungleNavigationCueSample& Sample)
	{
		const bool bRidgeSkyWindow = FJungleRidgeSlopeMountainEcosystemSpec::PreservesSkyWindow(Sample.TerrainSample);
		const bool bMountainSilhouette = FJungleRidgeSlopeMountainEcosystemSpec::IsMountainShoulderSilhouette(Sample.TerrainSample);
		const float RidgeSignal = FMath::Max(Sample.TerrainSample.GetMaskValue(TEXT("ridge_valley")), Sample.LandmarkContrast);

		if (!bRidgeSkyWindow && !bMountainSilhouette && RidgeSignal < MinimumLandmarkContrast)
		{
			return EJungleNavigationCueReadability::Unavailable;
		}

		return ResolveReadability(RidgeSignal, Sample.Environment);
	}

	static EJungleNavigationCueReadability ResolveSkyWindowCue(const FJungleNavigationCueSample& Sample)
	{
		const float SkySignal = Sample.TerrainSample.GetMaskValue(TEXT("sky_window")) * Sample.TerrainSample.GetMaskValue(TEXT("visibility"));
		return ResolveReadability(SkySignal, Sample.Environment);
	}

	static EJungleNavigationCueReadability ResolveSouthernSkyCue(const FJungleNavigationCueSample& Sample)
	{
		if (!Sample.Environment.bNight || (!Sample.Environment.bSouthernSkyVisible && !Sample.Environment.bMoonVisible))
		{
			return EJungleNavigationCueReadability::Unavailable;
		}

		const float SkySignal = FMath::Max(Sample.StarMoonVisibility, Sample.TerrainSample.GetMaskValue(TEXT("sky_window")));
		return ResolveReadability(SkySignal, Sample.Environment);
	}

	static EJungleNavigationCueReadability ResolveEnvironmentalSoundCue(const FJungleNavigationCueSample& Sample)
	{
		const float CreekSound = FJungleCreekRiparianWetValleyEcosystemSpec::IsInsideCreekInfluence(Sample.TerrainSample)
			? Sample.TerrainSample.GetMaskValue(TEXT("sound_propagation"))
			: 0.0f;
		const float GorgeSound = FJungleLowlandAlluvialGorgeEcosystemSpec::IsInsideGorgeInfluence(Sample.TerrainSample)
			? Sample.TerrainSample.GetMaskValue(TEXT("sound_propagation"))
			: 0.0f;
		const float OceanSound = Sample.Environment.bOceanAudible ? Sample.OceanSoundAudibility : 0.0f;
		return ResolveAuditoryReadability(FMath::Max3(CreekSound, GorgeSound, OceanSound), Sample.Environment);
	}

	static EJungleNavigationCueReadability ResolveTraversalAffordanceCue(const FJungleNavigationCueSample& Sample)
	{
		const EJungleTraversalMaskClass TraversalClass = FJungleEcosystemMaskSpec::ClassifyTraversal(
			Sample.TerrainSample.GetMaskValue(TEXT("hard_blocker")),
			Sample.TerrainSample.GetMaskValue(TEXT("soft_blocker")),
			Sample.TerrainSample.GetMaskValue(TEXT("affordance_zone")),
			Sample.TerrainSample.GetMaskValue(TEXT("false_affordance")));

		if (TraversalClass == EJungleTraversalMaskClass::Open)
		{
			return EJungleNavigationCueReadability::Unavailable;
		}

		const float TraversalSignal = FMath::Max(
			Sample.TerrainSample.GetMaskValue(TEXT("hard_blocker")),
			FMath::Max3(
				Sample.TerrainSample.GetMaskValue(TEXT("soft_blocker")),
				Sample.TerrainSample.GetMaskValue(TEXT("affordance_zone")),
				Sample.TerrainSample.GetMaskValue(TEXT("false_affordance"))));
		return ResolveReadability(TraversalSignal, Sample.Environment);
	}

	static TArray<FString> RequiredMaskNames()
	{
		TArray<FString> Names = {
			TEXT("height"),
			TEXT("slope"),
			TEXT("aspect"),
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
		return Names;
	}

	static TArray<FJungleNavigationCueContract> CueContracts()
	{
		TArray<FJungleNavigationCueContract> Contracts;

		FJungleNavigationCueContract Sun;
		Sun.Pillar = EJungleNavigationCuePillar::SunPosition;
		Sun.Modality = EJungleNavigationCueModality::VisualLight;
		Sun.RequiredMaskNames = { TEXT("sky_window"), TEXT("visibility"), TEXT("canopy") };
		Sun.ExpectedDegraders = { EJungleNavigationCueDegrader::DenseCanopy, EJungleNavigationCueDegrader::CloudCover, EJungleNavigationCueDegrader::GorgeOcclusion, EJungleNavigationCueDegrader::NightNoSun };
		Contracts.Add(Sun);

		FJungleNavigationCueContract Shadow;
		Shadow.Pillar = EJungleNavigationCuePillar::ShadowDirectionLength;
		Shadow.Modality = EJungleNavigationCueModality::VisualLight;
		Shadow.RequiredMaskNames = { TEXT("sky_window"), TEXT("visibility"), TEXT("canopy") };
		Shadow.ExpectedDegraders = { EJungleNavigationCueDegrader::DenseCanopy, EJungleNavigationCueDegrader::CloudCover, EJungleNavigationCueDegrader::RainMuffle, EJungleNavigationCueDegrader::NightNoSun };
		Contracts.Add(Shadow);

		FJungleNavigationCueContract LightColour;
		LightColour.Pillar = EJungleNavigationCuePillar::LightColour;
		LightColour.Modality = EJungleNavigationCueModality::VisualLight;
		LightColour.RequiredMaskNames = { TEXT("sky_window"), TEXT("visibility"), TEXT("canopy") };
		LightColour.ExpectedDegraders = { EJungleNavigationCueDegrader::DenseCanopy, EJungleNavigationCueDegrader::CloudCover, EJungleNavigationCueDegrader::GorgeOcclusion };
		Contracts.Add(LightColour);

		FJungleNavigationCueContract Creek;
		Creek.Pillar = EJungleNavigationCuePillar::CreekFlow;
		Creek.Modality = EJungleNavigationCueModality::VisualLandform;
		Creek.RequiredMaskNames = { TEXT("creek_distance"), TEXT("wetness"), TEXT("sound_propagation"), TEXT("affordance_zone") };
		Creek.ExpectedDegraders = { EJungleNavigationCueDegrader::RainMuffle, EJungleNavigationCueDegrader::LineOfSightBlocked };
		Contracts.Add(Creek);

		FJungleNavigationCueContract Slope;
		Slope.Pillar = EJungleNavigationCuePillar::SlopeGradient;
		Slope.Modality = EJungleNavigationCueModality::TraversalFriction;
		Slope.RequiredMaskNames = { TEXT("slope"), TEXT("footing_risk"), TEXT("hard_blocker"), TEXT("soft_blocker"), TEXT("affordance_zone") };
		Slope.ExpectedDegraders = { EJungleNavigationCueDegrader::DenseCanopy, EJungleNavigationCueDegrader::LineOfSightBlocked };
		Contracts.Add(Slope);

		FJungleNavigationCueContract Coast;
		Coast.Pillar = EJungleNavigationCuePillar::CoastOcean;
		Coast.Modality = EJungleNavigationCueModality::AuditoryEnvironment;
		Coast.RequiredMaskNames = { TEXT("coast_distance"), TEXT("sound_propagation"), TEXT("visibility"), TEXT("hard_blocker"), TEXT("false_affordance") };
		Coast.ExpectedDegraders = { EJungleNavigationCueDegrader::RainMuffle, EJungleNavigationCueDegrader::LineOfSightBlocked };
		Contracts.Add(Coast);

		FJungleNavigationCueContract Ridge;
		Ridge.Pillar = EJungleNavigationCuePillar::RidgeSpurSaddle;
		Ridge.Modality = EJungleNavigationCueModality::VisualLandform;
		Ridge.RequiredMaskNames = { TEXT("ridge_valley"), TEXT("slope"), TEXT("sky_window"), TEXT("visibility"), TEXT("affordance_zone") };
		Ridge.ExpectedDegraders = { EJungleNavigationCueDegrader::DenseCanopy, EJungleNavigationCueDegrader::GorgeOcclusion, EJungleNavigationCueDegrader::LineOfSightBlocked };
		Contracts.Add(Ridge);

		FJungleNavigationCueContract Mountain;
		Mountain.Pillar = EJungleNavigationCuePillar::MountainLandmark;
		Mountain.Modality = EJungleNavigationCueModality::VisualLandform;
		Mountain.RequiredMaskNames = { TEXT("height"), TEXT("ridge_valley"), TEXT("sky_window"), TEXT("visibility") };
		Mountain.ExpectedDegraders = { EJungleNavigationCueDegrader::DenseCanopy, EJungleNavigationCueDegrader::CloudCover, EJungleNavigationCueDegrader::GorgeOcclusion, EJungleNavigationCueDegrader::LineOfSightBlocked };
		Contracts.Add(Mountain);

		FJungleNavigationCueContract SkyWindow;
		SkyWindow.Pillar = EJungleNavigationCuePillar::SkyWindow;
		SkyWindow.Modality = EJungleNavigationCueModality::VisualLight;
		SkyWindow.RequiredMaskNames = { TEXT("sky_window"), TEXT("visibility"), TEXT("canopy") };
		SkyWindow.ExpectedDegraders = { EJungleNavigationCueDegrader::DenseCanopy, EJungleNavigationCueDegrader::CloudCover, EJungleNavigationCueDegrader::NoSkyWindow };
		Contracts.Add(SkyWindow);

		FJungleNavigationCueContract SouthernSky;
		SouthernSky.Pillar = EJungleNavigationCuePillar::SouthernSkyStarsMoon;
		SouthernSky.Modality = EJungleNavigationCueModality::VisualLight;
		SouthernSky.RequiredMaskNames = { TEXT("sky_window"), TEXT("visibility"), TEXT("canopy"), TEXT("night_danger") };
		SouthernSky.ExpectedDegraders = { EJungleNavigationCueDegrader::DenseCanopy, EJungleNavigationCueDegrader::CloudCover, EJungleNavigationCueDegrader::NoSkyWindow };
		Contracts.Add(SouthernSky);

		FJungleNavigationCueContract Sound;
		Sound.Pillar = EJungleNavigationCuePillar::EnvironmentalSound;
		Sound.Modality = EJungleNavigationCueModality::AuditoryEnvironment;
		Sound.RequiredMaskNames = { TEXT("sound_propagation"), TEXT("creek_distance"), TEXT("coast_distance"), TEXT("ridge_valley"), TEXT("visibility") };
		Sound.ExpectedDegraders = { EJungleNavigationCueDegrader::RainMuffle, EJungleNavigationCueDegrader::GorgeOcclusion };
		Contracts.Add(Sound);

		FJungleNavigationCueContract Traversal;
		Traversal.Pillar = EJungleNavigationCuePillar::TraversalAffordance;
		Traversal.Modality = EJungleNavigationCueModality::TraversalFriction;
		Traversal.RequiredMaskNames = { TEXT("slope"), TEXT("footing_risk"), TEXT("hard_blocker"), TEXT("soft_blocker"), TEXT("affordance_zone"), TEXT("false_affordance") };
		Traversal.ExpectedDegraders = { EJungleNavigationCueDegrader::DenseCanopy, EJungleNavigationCueDegrader::LineOfSightBlocked };
		Contracts.Add(Traversal);

		FJungleNavigationCueContract TrailMark;
		TrailMark.Pillar = EJungleNavigationCuePillar::PlayerMadeTrailMark;
		TrailMark.Modality = EJungleNavigationCueModality::RuntimePlayerMark;
		TrailMark.RequiredMaskNames = { TEXT("disturbance"), TEXT("visibility"), TEXT("canopy"), TEXT("wetness") };
		TrailMark.ExpectedDegraders = { EJungleNavigationCueDegrader::RainMuffle, EJungleNavigationCueDegrader::DenseCanopy, EJungleNavigationCueDegrader::LineOfSightBlocked };
		TrailMark.bRequiresRuntimePersistence = true;
		Contracts.Add(TrailMark);

		return Contracts;
	}

	static TArray<FString> RequiredDebugOutputNames()
	{
		return {
			TEXT("navigation_pillar_contracts"),
			TEXT("navigation_sun_position_signal"),
			TEXT("navigation_shadow_signal"),
			TEXT("navigation_light_colour_signal"),
			TEXT("navigation_creek_flow_signal"),
			TEXT("navigation_slope_signal"),
			TEXT("navigation_coast_ocean_signal"),
			TEXT("navigation_ridge_spur_saddle_signal"),
			TEXT("navigation_mountain_landmark_signal"),
			TEXT("navigation_sky_window_signal"),
			TEXT("navigation_southern_sky_signal"),
			TEXT("navigation_environmental_sound_signal"),
			TEXT("navigation_traversal_affordance_signal"),
			TEXT("navigation_player_trail_mark_runtime_contract"),
			TEXT("navigation_forbidden_ui_absence_check"),
		};
	}
};
