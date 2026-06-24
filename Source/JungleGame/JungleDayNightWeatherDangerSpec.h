#pragma once

#include "CoreMinimal.h"
#include "JungleLostStateGameplaySpec.h"

/** Prototype day/night phases. These are simulation states, not explicit player clock readouts. */
enum class EJungleDayNightPhase : uint8
{
	PreDawn,
	MorningLowSun,
	MiddayHighSun,
	AfternoonLowSun,
	Dusk,
	NightMoonOrStars,
	NightOvercast,
};

/** Prototype weather pressure states. These are gameplay readability contracts, not final weather simulation. */
enum class EJungleWeatherDangerState : uint8
{
	ClearReadable,
	HumidHaze,
	LightRain,
	HeavyRain,
	FogLowCloud,
	StormCell,
};

/** How day/night/weather attacks navigation and survival pressure. */
enum class EJungleWeatherNavigationAttack : uint8
{
	SunCueRemoved,
	ShadowCueFlattened,
	LightColourAmbiguous,
	SkyWindowBlocked,
	LandmarkVisibilityReduced,
	SoundMuffled,
	CreekSwollen,
	FootingWorsened,
	TrailMarksDegraded,
	KnownSafetyOccluded,
	NightDangerIncreased,
};

/** Engine-facing prototype hooks researched against UE atmospheric/audio systems. */
enum class EJungleAtmosphereImplementationHook : uint8
{
	SkyAtmosphere,
	SunDirectionalLight,
	MoonDirectionalLight,
	SkyLightRealtimeCapture,
	VolumetricCloud,
	ExponentialHeightFog,
	SoundAttenuationOcclusion,
	EnvironmentLightMixerValidation,
};

/** Internal danger output phases. These are not player-facing UI labels. */
enum class EJungleWeatherDangerPhase : uint8
{
	ReadableDay,
	DegradingRouteEvidence,
	WeatherAttacksNavigation,
	NightNavigationPressure,
	CriticalExposureWithoutRecovery,
};

/** Transient sky/time prototype input. The player may infer time from the world; this is not clock UI. */
struct FJungleDayNightSkySample
{
	EJungleDayNightPhase Phase = EJungleDayNightPhase::MiddayHighSun;
	float SunAltitudeSignal = 1.0f;
	float MoonAltitudeSignal = 0.0f;
	float StarVisibilitySignal = 0.0f;
	float SkyGlowSignal = 1.0f;
	float ShadowLengthSignal = 0.4f;
	float LightWarmthSignal = 0.4f;
	bool bSunAboveHorizon = true;
	bool bMoonAboveHorizon = false;
	bool bSouthernSkyVisible = false;

	bool HasSunNavigation() const
	{
		return bSunAboveHorizon && Phase != EJungleDayNightPhase::NightMoonOrStars && Phase != EJungleDayNightPhase::NightOvercast;
	}

	bool HasNightSkyNavigation() const
	{
		return Phase == EJungleDayNightPhase::NightMoonOrStars && (bMoonAboveHorizon || bSouthernSkyVisible || StarVisibilitySignal > 0.0f);
	}
};

/** Prototype weather sample consumed by danger and cue-degradation helpers. */
struct FJungleWeatherPrototypeSample
{
	EJungleWeatherDangerState WeatherState = EJungleWeatherDangerState::ClearReadable;
	float CloudOpacity = 0.0f;
	float RainIntensity = 0.0f;
	float FogDensity = 0.0f;
	float WindExposure = 0.0f;
	float AmbientSoundMuffle = 0.0f;
	float CreekSwellSignal = 0.0f;
	float TrailWashoutSignal = 0.0f;
	bool bLightningRiskPrototype = false;
	bool bStormGustPrototype = false;
};

/** Full input sample for PR #18 danger pressure. Values are internal/developer data only. */
struct FJungleDayNightWeatherDangerInput
{
	FJungleLostStateInputSample LostStateSample;
	FJungleDayNightSkySample SkySample;
	FJungleWeatherPrototypeSample WeatherSample;
	float KnownSafetyVisibility = 0.0f;
	float ShelterProximitySignal = 0.0f;
	float FireSmokeLineOfSightSignal = 0.0f;
	float BodyExposureSignal = 0.0f;
	bool bInsideKnownShelter = false;
	bool bUnderDenseCanopy = false;
	bool bInsideGorge = false;
};

/** Output of prototype pressure classification. Internal/debug only; not HUD state. */
struct FJungleDayNightWeatherDangerOutput
{
	EJungleWeatherDangerPhase DangerPhase = EJungleWeatherDangerPhase::ReadableDay;
	float NavigationCueLoss = 0.0f;
	float WeatherPressure = 0.0f;
	float NightPressure = 0.0f;
	float TerrainAmplifiedRisk = 0.0f;
	float RecoveryRelief = 0.0f;
	float FinalDangerScore = 0.0f;
};

/** Reviewable rule contract for day/night/weather danger prototype implementation. */
struct FJungleDayNightWeatherRuleContract
{
	EJungleWeatherNavigationAttack Attack = EJungleWeatherNavigationAttack::SunCueRemoved;
	TArray<FString> RequiredMaskNames;
	TArray<EJungleAtmosphereImplementationHook> EngineHooks;
	bool bCanIncreaseLostPressure = true;
	bool bCanReduceRecoveryEvidence = false;
	bool bCanIncreasePhysicalExposure = false;
	bool bDeveloperOnlyDebugOutput = true;
	bool bAllowsFinalWeatherSimulation = false;
	bool bAllowsFinalAudioMix = false;
	bool bAllowsFinalSkyArt = false;
	bool bAllowsClockOrExplicitTimeReadout = false;
	bool bAllowsMapCompassBearingGPSObjectiveUI = false;
	bool bAllowsOrientationConfidenceFeedback = false;
	bool bAllowsCombatOrChase = false;

	bool IsPrototypeOnly() const
	{
		return !bAllowsFinalWeatherSimulation
			&& !bAllowsFinalAudioMix
			&& !bAllowsFinalSkyArt
			&& !bAllowsClockOrExplicitTimeReadout
			&& !bAllowsMapCompassBearingGPSObjectiveUI
			&& !bAllowsOrientationConfidenceFeedback
			&& !bAllowsCombatOrChase;
	}

	bool IsReviewable() const
	{
		return IsPrototypeOnly()
			&& RequiredMaskNames.Num() > 0
			&& EngineHooks.Num() > 0
			&& (bCanIncreaseLostPressure || bCanReduceRecoveryEvidence || bCanIncreasePhysicalExposure);
	}
};

struct FJungleDayNightWeatherDangerSpec
{
	static constexpr int32 DayNightWeatherDangerVersion = 1;
	static constexpr float ReadableDayMaxDanger = 0.28f;
	static constexpr float DegradingEvidenceThreshold = 0.42f;
	static constexpr float WeatherAttackThreshold = 0.58f;
	static constexpr float NightPressureThreshold = 0.66f;
	static constexpr float CriticalExposureThreshold = 0.84f;
	static constexpr float RecoveryReliefWeight = 0.36f;
	static constexpr float ShelterReliefWeight = 0.44f;
	static constexpr float StormExposureWeight = 0.18f;
	static constexpr float NightCueLossWeight = 0.28f;
	static constexpr float WeatherCueLossWeight = 0.30f;
	static constexpr float TerrainAmplifierWeight = 0.24f;

	static bool AcceptsPlayerMap()
	{
		return false;
	}

	static bool AcceptsCompassBearingGPSOrObjectiveNavigation()
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

	static bool AcceptsFullDamageDeathSystem()
	{
		return false;
	}

	static bool AcceptsCombatChaseOrHorrorEnemySystem()
	{
		return false;
	}

	static float ClampSignal(float Value)
	{
		return FMath::Clamp(Value, FJungleEcosystemMaskSpec::NormalizedMin, FJungleEcosystemMaskSpec::NormalizedMax);
	}

	static float ResolveDaylightNavigationSignal(const FJungleDayNightSkySample& Sky)
	{
		if (!Sky.HasSunNavigation())
		{
			return 0.0f;
		}

		const float SunSignal = FMath::Max(Sky.SunAltitudeSignal, Sky.SkyGlowSignal);
		const float ShadowLightSignal = FMath::Max(Sky.ShadowLengthSignal, Sky.LightWarmthSignal);
		return ClampSignal(FMath::Max(SunSignal, ShadowLightSignal));
	}

	static float ResolveNightSkyNavigationSignal(const FJungleDayNightSkySample& Sky)
	{
		if (!Sky.HasNightSkyNavigation())
		{
			return 0.0f;
		}

		return ClampSignal(FMath::Max3(Sky.MoonAltitudeSignal, Sky.StarVisibilitySignal, Sky.SkyGlowSignal));
	}

	static float ResolveSkyOcclusionPressure(const FJungleWeatherPrototypeSample& Weather, bool bUnderDenseCanopy, bool bInsideGorge)
	{
		float Pressure = FMath::Max(Weather.CloudOpacity, Weather.FogDensity);

		if (bUnderDenseCanopy)
		{
			Pressure += 0.24f;
		}

		if (bInsideGorge)
		{
			Pressure += 0.22f;
		}

		return ClampSignal(Pressure);
	}

	static float ResolveWeatherPressure(const FJungleWeatherPrototypeSample& Weather)
	{
		float Pressure = FMath::Max3(Weather.CloudOpacity, Weather.RainIntensity, Weather.FogDensity);
		Pressure = FMath::Max(Pressure, Weather.AmbientSoundMuffle);
		Pressure = FMath::Max(Pressure, Weather.CreekSwellSignal);

		if (Weather.bLightningRiskPrototype)
		{
			Pressure += StormExposureWeight;
		}

		if (Weather.bStormGustPrototype)
		{
			Pressure += StormExposureWeight * 0.75f;
		}

		return ClampSignal(Pressure);
	}

	static float ResolveNightPressure(const FJungleDayNightSkySample& Sky, const FJungleWeatherPrototypeSample& Weather)
	{
		const bool bNightPhase = Sky.Phase == EJungleDayNightPhase::NightMoonOrStars || Sky.Phase == EJungleDayNightPhase::NightOvercast;
		if (!bNightPhase)
		{
			return 0.0f;
		}

		const float NightSkyRelief = ResolveNightSkyNavigationSignal(Sky) * 0.42f;
		const float OvercastPenalty = Sky.Phase == EJungleDayNightPhase::NightOvercast ? 0.34f : 0.0f;
		return ClampSignal(0.74f + OvercastPenalty + Weather.CloudOpacity * 0.22f + Weather.RainIntensity * 0.18f - NightSkyRelief);
	}

	static float ResolveTerrainAmplifiedRisk(const FJungleDayNightWeatherDangerInput& Input)
	{
		const FJunglePCGBiomeInputSample& Terrain = Input.LostStateSample.NavigationSample.TerrainSample;
		const float BlockerSignal = FMath::Max3(
			Terrain.GetMaskValue(TEXT("hard_blocker")),
			Terrain.GetMaskValue(TEXT("soft_blocker")),
			Terrain.GetMaskValue(TEXT("false_affordance")));
		const float FootingSignal = Terrain.GetMaskValue(TEXT("footing_risk"));
		const float CreekSwellSignal = Input.WeatherSample.CreekSwellSignal;
		const float SlopeSignal = ClampSignal(Terrain.SlopeDegrees / FJungleMacroTerrainSpec::HardBlockerSlopeDegrees);
		return ClampSignal(FMath::Max(FMath::Max3(BlockerSignal, FootingSignal, CreekSwellSignal), SlopeSignal));
	}

	static float ResolveTrailMarkWeatherLoss(const FJungleDayNightWeatherDangerInput& Input)
	{
		const float RainWashout = FMath::Max(Input.WeatherSample.RainIntensity, Input.WeatherSample.TrailWashoutSignal);
		const float VisibilityLoss = 1.0f - ClampSignal(Input.LostStateSample.TrailMarkVisibility);
		return ClampSignal(FMath::Max(RainWashout, VisibilityLoss));
	}

	static float ResolveKnownSafetyRelief(const FJungleDayNightWeatherDangerInput& Input)
	{
		if (Input.bInsideKnownShelter)
		{
			return 1.0f;
		}

		const float VisibleSafety = FMath::Max3(
			Input.KnownSafetyVisibility,
			Input.ShelterProximitySignal,
			Input.FireSmokeLineOfSightSignal);
		return ClampSignal(VisibleSafety);
	}

	static float ResolveNavigationCueLoss(const FJungleDayNightWeatherDangerInput& Input)
	{
		const float DaylightSignal = ResolveDaylightNavigationSignal(Input.SkySample);
		const float NightSkySignal = ResolveNightSkyNavigationSignal(Input.SkySample);
		const float SkyOcclusion = ResolveSkyOcclusionPressure(Input.WeatherSample, Input.bUnderDenseCanopy, Input.bInsideGorge);
		const float TrailLoss = ResolveTrailMarkWeatherLoss(Input);
		const float CueLoss = 1.0f - FMath::Max(DaylightSignal, NightSkySignal);
		return ClampSignal(FMath::Max3(CueLoss, SkyOcclusion, TrailLoss));
	}

	static FJungleNavigationEnvironmentalState ToNavigationEnvironment(const FJungleDayNightWeatherDangerInput& Input)
	{
		FJungleNavigationEnvironmentalState Environment;
		Environment.bDaylight = Input.SkySample.HasSunNavigation();
		Environment.bNight = Input.SkySample.Phase == EJungleDayNightPhase::NightMoonOrStars || Input.SkySample.Phase == EJungleDayNightPhase::NightOvercast;
		Environment.bClearSky = Input.WeatherSample.CloudOpacity < 0.35f && Input.WeatherSample.FogDensity < 0.35f;
		Environment.bDenseCanopy = Input.bUnderDenseCanopy;
		Environment.bCloudCover = Input.WeatherSample.CloudOpacity >= 0.45f;
		Environment.bRain = Input.WeatherSample.RainIntensity >= 0.35f;
		Environment.bInsideGorge = Input.bInsideGorge;
		Environment.bSkyWindowVisible = Input.LostStateSample.NavigationSample.TerrainSample.GetMaskValue(TEXT("sky_window")) >= FJungleNavigationPillarsSpec::MinimumSunSkyWindow;
		Environment.bMoonVisible = Input.SkySample.bMoonAboveHorizon && Input.SkySample.MoonAltitudeSignal > 0.0f;
		Environment.bSouthernSkyVisible = Input.SkySample.bSouthernSkyVisible;
		Environment.bOceanAudible = Input.LostStateSample.NavigationSample.Environment.bOceanAudible && Input.WeatherSample.AmbientSoundMuffle < 0.78f;
		Environment.bLandmarkLineOfSight = Input.KnownSafetyVisibility >= 0.45f;
		return Environment;
	}

	static FJungleDayNightWeatherDangerOutput ResolveDangerOutput(const FJungleDayNightWeatherDangerInput& Input)
	{
		FJungleDayNightWeatherDangerOutput Output;
		Output.NavigationCueLoss = ResolveNavigationCueLoss(Input);
		Output.WeatherPressure = ResolveWeatherPressure(Input.WeatherSample);
		Output.NightPressure = ResolveNightPressure(Input.SkySample, Input.WeatherSample);
		Output.TerrainAmplifiedRisk = ResolveTerrainAmplifiedRisk(Input);
		Output.RecoveryRelief = ResolveKnownSafetyRelief(Input) * ShelterReliefWeight
			+ FJungleLostStateGameplaySpec::ResolveRecoveryEvidenceSignal(Input.LostStateSample) * RecoveryReliefWeight;

		float DangerScore = 0.0f;
		DangerScore += Output.NavigationCueLoss * WeatherCueLossWeight;
		DangerScore += Output.WeatherPressure * WeatherCueLossWeight;
		DangerScore += Output.NightPressure * NightCueLossWeight;
		DangerScore += Output.TerrainAmplifiedRisk * TerrainAmplifierWeight;
		DangerScore += Input.BodyExposureSignal * StormExposureWeight;
		DangerScore -= Output.RecoveryRelief;
		Output.FinalDangerScore = ClampSignal(DangerScore);

		if (Output.FinalDangerScore >= CriticalExposureThreshold && Output.RecoveryRelief < 0.25f)
		{
			Output.DangerPhase = EJungleWeatherDangerPhase::CriticalExposureWithoutRecovery;
		}
		else if (Output.NightPressure >= NightPressureThreshold)
		{
			Output.DangerPhase = EJungleWeatherDangerPhase::NightNavigationPressure;
		}
		else if (Output.WeatherPressure >= WeatherAttackThreshold)
		{
			Output.DangerPhase = EJungleWeatherDangerPhase::WeatherAttacksNavigation;
		}
		else if (Output.NavigationCueLoss >= DegradingEvidenceThreshold)
		{
			Output.DangerPhase = EJungleWeatherDangerPhase::DegradingRouteEvidence;
		}
		else
		{
			Output.DangerPhase = EJungleWeatherDangerPhase::ReadableDay;
		}

		return Output;
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

	static TArray<FString> RequiredEngineHookNames()
	{
		return {
			TEXT("SkyAtmosphere"),
			TEXT("SunDirectionalLight_AtmosphereSunLightIndex0"),
			TEXT("MoonDirectionalLight_AtmosphereSunLightIndex1"),
			TEXT("SkyLight_RealTimeCapture"),
			TEXT("VolumetricCloud_Prototype"),
			TEXT("ExponentialHeightFog_Prototype"),
			TEXT("SoundAttenuation_OcclusionPrototype"),
			TEXT("EnvironmentLightMixer_ValidationPath"),
		};
	}

	static TArray<FJungleDayNightWeatherRuleContract> RuleContracts()
	{
		TArray<FJungleDayNightWeatherRuleContract> Rules;

		FJungleDayNightWeatherRuleContract SunRemoved;
		SunRemoved.Attack = EJungleWeatherNavigationAttack::SunCueRemoved;
		SunRemoved.RequiredMaskNames = { TEXT("sky_window"), TEXT("visibility"), TEXT("canopy"), TEXT("night_danger") };
		SunRemoved.EngineHooks = { EJungleAtmosphereImplementationHook::SkyAtmosphere, EJungleAtmosphereImplementationHook::SunDirectionalLight };
		SunRemoved.bCanIncreaseLostPressure = true;
		Rules.Add(SunRemoved);

		FJungleDayNightWeatherRuleContract ShadowFlattened;
		ShadowFlattened.Attack = EJungleWeatherNavigationAttack::ShadowCueFlattened;
		ShadowFlattened.RequiredMaskNames = { TEXT("sky_window"), TEXT("visibility"), TEXT("canopy") };
		ShadowFlattened.EngineHooks = { EJungleAtmosphereImplementationHook::SkyAtmosphere, EJungleAtmosphereImplementationHook::SunDirectionalLight, EJungleAtmosphereImplementationHook::VolumetricCloud };
		ShadowFlattened.bCanIncreaseLostPressure = true;
		Rules.Add(ShadowFlattened);

		FJungleDayNightWeatherRuleContract SkyBlocked;
		SkyBlocked.Attack = EJungleWeatherNavigationAttack::SkyWindowBlocked;
		SkyBlocked.RequiredMaskNames = { TEXT("sky_window"), TEXT("visibility"), TEXT("canopy"), TEXT("ridge_valley") };
		SkyBlocked.EngineHooks = { EJungleAtmosphereImplementationHook::VolumetricCloud, EJungleAtmosphereImplementationHook::ExponentialHeightFog };
		SkyBlocked.bCanIncreaseLostPressure = true;
		SkyBlocked.bCanReduceRecoveryEvidence = true;
		Rules.Add(SkyBlocked);

		FJungleDayNightWeatherRuleContract LandmarkReduced;
		LandmarkReduced.Attack = EJungleWeatherNavigationAttack::LandmarkVisibilityReduced;
		LandmarkReduced.RequiredMaskNames = { TEXT("height"), TEXT("ridge_valley"), TEXT("visibility"), TEXT("sky_window") };
		LandmarkReduced.EngineHooks = { EJungleAtmosphereImplementationHook::SkyAtmosphere, EJungleAtmosphereImplementationHook::VolumetricCloud, EJungleAtmosphereImplementationHook::ExponentialHeightFog };
		LandmarkReduced.bCanIncreaseLostPressure = true;
		LandmarkReduced.bCanReduceRecoveryEvidence = true;
		Rules.Add(LandmarkReduced);

		FJungleDayNightWeatherRuleContract SoundMuffled;
		SoundMuffled.Attack = EJungleWeatherNavigationAttack::SoundMuffled;
		SoundMuffled.RequiredMaskNames = { TEXT("sound_propagation"), TEXT("creek_distance"), TEXT("coast_distance"), TEXT("visibility") };
		SoundMuffled.EngineHooks = { EJungleAtmosphereImplementationHook::SoundAttenuationOcclusion };
		SoundMuffled.bCanIncreaseLostPressure = true;
		SoundMuffled.bCanReduceRecoveryEvidence = true;
		Rules.Add(SoundMuffled);

		FJungleDayNightWeatherRuleContract CreekSwollen;
		CreekSwollen.Attack = EJungleWeatherNavigationAttack::CreekSwollen;
		CreekSwollen.RequiredMaskNames = { TEXT("creek_distance"), TEXT("wetness"), TEXT("footing_risk"), TEXT("hard_blocker"), TEXT("soft_blocker") };
		CreekSwollen.EngineHooks = { EJungleAtmosphereImplementationHook::VolumetricCloud, EJungleAtmosphereImplementationHook::SoundAttenuationOcclusion };
		CreekSwollen.bCanIncreaseLostPressure = true;
		CreekSwollen.bCanIncreasePhysicalExposure = true;
		Rules.Add(CreekSwollen);

		FJungleDayNightWeatherRuleContract FootingWorsened;
		FootingWorsened.Attack = EJungleWeatherNavigationAttack::FootingWorsened;
		FootingWorsened.RequiredMaskNames = { TEXT("slope"), TEXT("wetness"), TEXT("footing_risk"), TEXT("soft_blocker"), TEXT("false_affordance") };
		FootingWorsened.EngineHooks = { EJungleAtmosphereImplementationHook::VolumetricCloud };
		FootingWorsened.bCanIncreaseLostPressure = true;
		FootingWorsened.bCanIncreasePhysicalExposure = true;
		Rules.Add(FootingWorsened);

		FJungleDayNightWeatherRuleContract TrailMarks;
		TrailMarks.Attack = EJungleWeatherNavigationAttack::TrailMarksDegraded;
		TrailMarks.RequiredMaskNames = { TEXT("disturbance"), TEXT("visibility"), TEXT("wetness"), TEXT("canopy") };
		TrailMarks.EngineHooks = { EJungleAtmosphereImplementationHook::VolumetricCloud, EJungleAtmosphereImplementationHook::ExponentialHeightFog };
		TrailMarks.bCanIncreaseLostPressure = true;
		TrailMarks.bCanReduceRecoveryEvidence = true;
		Rules.Add(TrailMarks);

		FJungleDayNightWeatherRuleContract KnownSafety;
		KnownSafety.Attack = EJungleWeatherNavigationAttack::KnownSafetyOccluded;
		KnownSafety.RequiredMaskNames = { TEXT("visibility"), TEXT("sound_propagation"), TEXT("sky_window"), TEXT("canopy") };
		KnownSafety.EngineHooks = { EJungleAtmosphereImplementationHook::SkyAtmosphere, EJungleAtmosphereImplementationHook::VolumetricCloud, EJungleAtmosphereImplementationHook::SoundAttenuationOcclusion };
		KnownSafety.bCanReduceRecoveryEvidence = true;
		Rules.Add(KnownSafety);

		FJungleDayNightWeatherRuleContract NightDanger;
		NightDanger.Attack = EJungleWeatherNavigationAttack::NightDangerIncreased;
		NightDanger.RequiredMaskNames = { TEXT("night_danger"), TEXT("sky_window"), TEXT("visibility"), TEXT("sound_propagation"), TEXT("footing_risk") };
		NightDanger.EngineHooks = { EJungleAtmosphereImplementationHook::SkyAtmosphere, EJungleAtmosphereImplementationHook::MoonDirectionalLight, EJungleAtmosphereImplementationHook::SkyLightRealtimeCapture };
		NightDanger.bCanIncreaseLostPressure = true;
		NightDanger.bCanIncreasePhysicalExposure = true;
		Rules.Add(NightDanger);

		return Rules;
	}

	static TArray<FString> RequiredDebugOutputNames()
	{
		return {
			TEXT("day_night_weather_rule_contracts"),
			TEXT("daylight_navigation_signal"),
			TEXT("night_sky_navigation_signal"),
			TEXT("sky_occlusion_pressure"),
			TEXT("weather_pressure"),
			TEXT("night_pressure"),
			TEXT("terrain_amplified_weather_risk"),
			TEXT("trail_mark_weather_loss"),
			TEXT("known_safety_weather_relief"),
			TEXT("final_weather_danger_score"),
			TEXT("weather_forbidden_ui_absence_check"),
		};
	}
};
