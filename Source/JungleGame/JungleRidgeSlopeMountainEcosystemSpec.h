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

enum class EJG_OrientationCueCategory : uint8
{
	TerrainBackbone,
	DrainageFlow,
	RidgeSpurSaddle,
	SlopeFeel,
	SkylineSilhouette,
	CanopyGap,
	SkyWindow,
	SolarShadow,
	NightCelestial,
	WeatherAtmosphere,
	AcousticDirection,
	VegetationTransition,
	TraversalRisk,
	CoastalMacroCue,
};

enum class EJG_TerrainForm : uint8
{
	Unknown,
	MainRidge,
	Spur,
	Saddle,
	UpperSlope,
	MidSlope,
	LowerSlope,
	SlopeBreak,
	Bench,
	Gully,
	Gorge,
	ValleyFloor,
	CreekCorridor,
	CreekJunction,
	WaterfallApproach,
	CoastalEscarpment,
	BeachExit,
	Headland,
};

enum class EJG_CanopyCueState : uint8
{
	Closed,
	DenseFiltered,
	BrokenFiltered,
	LocalGap,
	StormGap,
	RidgeThin,
	SaddleWindow,
	GorgeSlot,
	CoastalOpen,
};

enum class EJG_CueDegradationState : uint8
{
	ClearReadable,
	FilteredReadable,
	Intermittent,
	Ambiguous,
	Occluded,
	Distorted,
	Suppressed,
};

enum class EJG_WeatherCueState : uint8
{
	Clear,
	BrokenCloud,
	OvercastBright,
	OvercastDark,
	LightRain,
	HeavyRain,
	Mist,
	DenseFog,
	Storm,
	PostRainClearing,
};

enum class EJG_SolarCueState : uint8
{
	None,
	DiffuseBrightnessGradient,
	FilteredCanopyMottle,
	PartialSunDisk,
	DirectSunAndShadow,
	LowAngleRidgeLight,
	StormSuppressed,
};

enum class EJG_ShadowCueState : uint8
{
	None,
	DiffuseOnly,
	MottledUnreliable,
	ShortLocalReliable,
	LongDirectionalReliable,
	DistortedByTerrain,
};

enum class EJG_NightCueState : uint8
{
	BlackCanopy,
	MoonFilteredCanopy,
	MoonGapVisible,
	StarWindowVisible,
	RidgeSilhouetteVisible,
	CoastalSkyGlowVisible,
	StormNightSuppressed,
};

enum class EJG_SkyWindowClass : uint8
{
	None,
	PinHole,
	FilteredCanopy,
	NarrowSlot,
	LocalGap,
	RidgeWindow,
	SaddleWindow,
	CoastalWindow,
};

enum class EJG_OrientationDebugView : uint8
{
	All,
	TerrainBackbone,
	RidgeSpurSaddle,
	DrainageFlow,
	CanopyClosure,
	SkyWindow,
	SolarShadow,
	NightSky,
	WeatherDegradation,
	AcousticReach,
	VegetationTransition,
	TraversalPressure,
	CueFloor,
	DeveloperAudit,
};

enum class EJG_OrientationValidationPointType : uint8
{
	RidgeCrest,
	SpurBranch,
	Saddle,
	Gully,
	Gorge,
	CanopyGap,
	CreekJunction,
	CoastalExit,
	FoggedRidge,
	NightSkyWindow,
};

struct DA_JGOrientationCueProfile
{
	FName ProfileId;
	EJG_OrientationCueCategory CueCategory = EJG_OrientationCueCategory::TerrainBackbone;
	EJG_TerrainForm TerrainForm = EJG_TerrainForm::Unknown;
	EJG_CanopyCueState CanopyCueState = EJG_CanopyCueState::Closed;
	EJG_WeatherCueState WeatherCueState = EJG_WeatherCueState::Clear;
	float MinReadableDistance = 0.0f;
	float MaxReadableDistance = 80.0f;
	float OcclusionSensitivity = 0.5f;
	float WeatherSensitivity = 0.5f;
	TArray<FName> EvidenceTags;
	TArray<FName> ScreenshotValidationTags;
	bool bDeveloperOnly = true;
};

struct DA_JGSkylineAnchorProfile
{
	FName AnchorId;
	EJG_TerrainForm TerrainForm = EJG_TerrainForm::MainRidge;
	EJG_SkyWindowClass RequiredWindow = EJG_SkyWindowClass::RidgeWindow;
	float MinimumSilhouetteVisibility = 0.68f;
	float MinimumHorizonFraction = 0.15f;
	bool bRequiresHLODReview = true;
};

struct DA_JGCanopyCueProfile
{
	FName ProfileId;
	EJG_CanopyCueState CanopyCueState = EJG_CanopyCueState::DenseFiltered;
	float SkyVisibleFraction = 0.0f;
	float DirectSunVisibleFraction = 0.0f;
	float HorizonVisibleFraction = 0.0f;
	float SilhouetteVisibleFraction = 0.0f;
	float MoonStarVisibleFraction = 0.0f;
	float CanopyOcclusionFraction = 1.0f;
};

struct DA_JGWeatherDegradationProfile
{
	FName ProfileId;
	EJG_WeatherCueState WeatherCueState = EJG_WeatherCueState::Clear;
	float WeatherVisibilityMeters = 1000.0f;
	float FogDensityNormalized = 0.0f;
	float RainIntensityNormalized = 0.0f;
	float CloudCoverNormalized = 0.0f;
	float DirectSunTransmission = 1.0f;
	float DiffuseSkyTransmission = 1.0f;
	float DistantSoundSuppression = 0.0f;
};

struct DA_JGTraversalPressureProfile
{
	FName ProfileId;
	EJG_TerrainForm TerrainForm = EJG_TerrainForm::Unknown;
	float SlopeDegrees = 0.0f;
	float FootingRisk = 0.0f;
	float VegetationResistance = 0.0f;
	float WindExposure = 0.0f;
	EJG_CueDegradationState CueState = EJG_CueDegradationState::FilteredReadable;
};

struct BP_JGOrientationValidationPoint
{
	FName ValidationPointId;
	FVector WorldLocation = FVector::ZeroVector;
	EJG_OrientationValidationPointType Type = EJG_OrientationValidationPointType::RidgeCrest;
	EJG_TerrainForm TerrainForm = EJG_TerrainForm::MainRidge;
	EJG_CanopyCueState CanopyCueState = EJG_CanopyCueState::RidgeThin;
	EJG_SkyWindowClass SkyWindowClass = EJG_SkyWindowClass::RidgeWindow;
	EJG_WeatherCueState WeatherCueState = EJG_WeatherCueState::Clear;
	TArray<EJG_OrientationCueCategory> ReadableCueCategories;
	TArray<EJG_OrientationCueCategory> SuppressedCueCategories;
	EJG_OrientationCueCategory DominantRecoveryCue = EJG_OrientationCueCategory::TerrainBackbone;
	bool bScreenshotRequired = true;
	bool bDeveloperOnly = true;

	bool PassesCueFloor() const
	{
		return ReadableCueCategories.Num() >= 2;
	}
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
	static constexpr int32 EcosystemPassVersion = 2;
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

	static EJG_CueDegradationState ResolveCueDegradation(float CueStrength, float CanopyTransmission, float WeatherVisibilityFactor, float LocalOcclusion)
	{
		const float Score = CueStrength * CanopyTransmission * WeatherVisibilityFactor * (1.0f - LocalOcclusion);

		if (Score >= 0.80f)
		{
			return EJG_CueDegradationState::ClearReadable;
		}

		if (Score >= 0.55f)
		{
			return EJG_CueDegradationState::FilteredReadable;
		}

		if (Score >= 0.35f)
		{
			return EJG_CueDegradationState::Intermittent;
		}

		if (Score >= 0.20f)
		{
			return EJG_CueDegradationState::Ambiguous;
		}

		return LocalOcclusion >= 0.70f ? EJG_CueDegradationState::Occluded : EJG_CueDegradationState::Suppressed;
	}

	static EJG_SolarCueState ResolveSolarCueState(const DA_JGCanopyCueProfile& Canopy, const DA_JGWeatherDegradationProfile& Weather)
	{
		if (Weather.RainIntensityNormalized >= 0.80f || Weather.CloudCoverNormalized >= 0.92f)
		{
			return EJG_SolarCueState::StormSuppressed;
		}

		if (Canopy.DirectSunVisibleFraction >= 0.70f && Weather.DirectSunTransmission >= 0.60f)
		{
			return EJG_SolarCueState::DirectSunAndShadow;
		}

		if (Canopy.DirectSunVisibleFraction >= 0.35f)
		{
			return EJG_SolarCueState::PartialSunDisk;
		}

		if (Canopy.SkyVisibleFraction >= 0.25f)
		{
			return EJG_SolarCueState::FilteredCanopyMottle;
		}

		return Weather.DiffuseSkyTransmission > 0.20f ? EJG_SolarCueState::DiffuseBrightnessGradient : EJG_SolarCueState::None;
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

		if (Sample.GetMaskValue(TEXT("affordance_zone")) >= MinimumSaddleAffordance
			&& Sample.GetMaskValue(TEXT("sky_window")) >= MinimumSkyWindow * 0.75f)
		{
			return EJungleRidgeSlopeMountainZone::SaddleTransitionAffordance;
		}

		if (Sample.GetMaskValue(TEXT("affordance_zone")) >= MinimumSpurAffordance)
		{
			return EJungleRidgeSlopeMountainZone::SpurMovementAffordance;
		}

		return EJungleRidgeSlopeMountainZone::SlopeBreakRouteDecision;
	}

	static TArray<FString> RequiredMaskNames()
	{
		return {
			TEXT("ridge_valley"),
			TEXT("sky_window"),
			TEXT("visibility"),
			TEXT("canopy"),
			TEXT("aspect"),
			TEXT("sound_propagation"),
			TEXT("footing_risk"),
			TEXT("hard_blocker"),
			TEXT("soft_blocker"),
			TEXT("affordance_zone"),
			TEXT("false_affordance"),
			TEXT("M_Terrain_MainRidgeMask"),
			TEXT("M_Terrain_SpurMask"),
			TEXT("M_Terrain_SaddleMask"),
			TEXT("M_Terrain_GullyMask"),
			TEXT("M_Terrain_GorgeMask"),
			TEXT("M_Terrain_SlopeBreakMask"),
			TEXT("M_Canopy_ClosedMask"),
			TEXT("M_Canopy_GapMask"),
			TEXT("M_Canopy_SkyWindowMask"),
			TEXT("M_Canopy_RidgeThinMask"),
			TEXT("M_Veg_CreekMarginMask"),
			TEXT("M_Veg_CanopyGapRegrowthMask"),
			TEXT("M_Exposure_WindRidgeMask"),
		};
	}

	static TArray<FString> DebugViewNames()
	{
		return {
			TEXT("DBG_OrientationCue_All"),
			TEXT("DBG_OrientationCue_TerrainBackbone"),
			TEXT("DBG_OrientationCue_RidgeSpurSaddle"),
			TEXT("DBG_OrientationCue_DrainageFlow"),
			TEXT("DBG_OrientationCue_CanopyClosure"),
			TEXT("DBG_OrientationCue_SkyWindow"),
			TEXT("DBG_OrientationCue_SolarShadow"),
			TEXT("DBG_OrientationCue_NightSky"),
			TEXT("DBG_OrientationCue_WeatherDegradation"),
			TEXT("DBG_OrientationCue_AcousticReach"),
			TEXT("DBG_OrientationCue_VegetationTransition"),
			TEXT("DBG_OrientationCue_TraversalPressure"),
			TEXT("DBG_OrientationCue_CueFloor"),
			TEXT("DBG_OrientationCue_DeveloperAudit"),
		};
	}

	static TArray<BP_JGOrientationValidationPoint> DefaultValidationPoints()
	{
		return {
			{ TEXT("VP_RidgeCrest"), FVector::ZeroVector, EJG_OrientationValidationPointType::RidgeCrest, EJG_TerrainForm::MainRidge, EJG_CanopyCueState::RidgeThin, EJG_SkyWindowClass::RidgeWindow, EJG_WeatherCueState::Clear, { EJG_OrientationCueCategory::TerrainBackbone, EJG_OrientationCueCategory::SkyWindow, EJG_OrientationCueCategory::AcousticDirection }, {}, EJG_OrientationCueCategory::TerrainBackbone, true, true },
			{ TEXT("VP_SpurBranch"), FVector::ZeroVector, EJG_OrientationValidationPointType::SpurBranch, EJG_TerrainForm::Spur, EJG_CanopyCueState::BrokenFiltered, EJG_SkyWindowClass::FilteredCanopy, EJG_WeatherCueState::Clear, { EJG_OrientationCueCategory::RidgeSpurSaddle, EJG_OrientationCueCategory::SlopeFeel }, {}, EJG_OrientationCueCategory::RidgeSpurSaddle, true, true },
			{ TEXT("VP_Saddle"), FVector::ZeroVector, EJG_OrientationValidationPointType::Saddle, EJG_TerrainForm::Saddle, EJG_CanopyCueState::SaddleWindow, EJG_SkyWindowClass::SaddleWindow, EJG_WeatherCueState::BrokenCloud, { EJG_OrientationCueCategory::RidgeSpurSaddle, EJG_OrientationCueCategory::SkyWindow, EJG_OrientationCueCategory::WeatherAtmosphere }, {}, EJG_OrientationCueCategory::SkyWindow, true, true },
			{ TEXT("VP_Gully"), FVector::ZeroVector, EJG_OrientationValidationPointType::Gully, EJG_TerrainForm::Gully, EJG_CanopyCueState::Closed, EJG_SkyWindowClass::None, EJG_WeatherCueState::LightRain, { EJG_OrientationCueCategory::DrainageFlow, EJG_OrientationCueCategory::VegetationTransition }, { EJG_OrientationCueCategory::SkyWindow }, EJG_OrientationCueCategory::DrainageFlow, true, true },
			{ TEXT("VP_Gorge"), FVector::ZeroVector, EJG_OrientationValidationPointType::Gorge, EJG_TerrainForm::Gorge, EJG_CanopyCueState::GorgeSlot, EJG_SkyWindowClass::NarrowSlot, EJG_WeatherCueState::Mist, { EJG_OrientationCueCategory::DrainageFlow, EJG_OrientationCueCategory::AcousticDirection }, { EJG_OrientationCueCategory::SkylineSilhouette }, EJG_OrientationCueCategory::AcousticDirection, true, true },
			{ TEXT("VP_CanopyGap"), FVector::ZeroVector, EJG_OrientationValidationPointType::CanopyGap, EJG_TerrainForm::SlopeBreak, EJG_CanopyCueState::LocalGap, EJG_SkyWindowClass::LocalGap, EJG_WeatherCueState::Clear, { EJG_OrientationCueCategory::CanopyGap, EJG_OrientationCueCategory::SolarShadow }, {}, EJG_OrientationCueCategory::CanopyGap, true, true },
			{ TEXT("VP_CreekJunction"), FVector::ZeroVector, EJG_OrientationValidationPointType::CreekJunction, EJG_TerrainForm::CreekJunction, EJG_CanopyCueState::DenseFiltered, EJG_SkyWindowClass::FilteredCanopy, EJG_WeatherCueState::PostRainClearing, { EJG_OrientationCueCategory::DrainageFlow, EJG_OrientationCueCategory::AcousticDirection }, {}, EJG_OrientationCueCategory::DrainageFlow, true, true },
			{ TEXT("VP_CoastalExit"), FVector::ZeroVector, EJG_OrientationValidationPointType::CoastalExit, EJG_TerrainForm::BeachExit, EJG_CanopyCueState::CoastalOpen, EJG_SkyWindowClass::CoastalWindow, EJG_WeatherCueState::Clear, { EJG_OrientationCueCategory::CoastalMacroCue, EJG_OrientationCueCategory::SkyWindow }, {}, EJG_OrientationCueCategory::CoastalMacroCue, true, true },
			{ TEXT("VP_FoggedRidge"), FVector::ZeroVector, EJG_OrientationValidationPointType::FoggedRidge, EJG_TerrainForm::MainRidge, EJG_CanopyCueState::RidgeThin, EJG_SkyWindowClass::RidgeWindow, EJG_WeatherCueState::DenseFog, { EJG_OrientationCueCategory::TerrainBackbone, EJG_OrientationCueCategory::TraversalRisk }, { EJG_OrientationCueCategory::SkylineSilhouette }, EJG_OrientationCueCategory::TerrainBackbone, true, true },
			{ TEXT("VP_NightSkyWindow"), FVector::ZeroVector, EJG_OrientationValidationPointType::NightSkyWindow, EJG_TerrainForm::Saddle, EJG_CanopyCueState::SaddleWindow, EJG_SkyWindowClass::SaddleWindow, EJG_WeatherCueState::Clear, { EJG_OrientationCueCategory::NightCelestial, EJG_OrientationCueCategory::AcousticDirection }, { EJG_OrientationCueCategory::SolarShadow }, EJG_OrientationCueCategory::NightCelestial, true, true },
		};
	}

	static TArray<FJungleRidgeSlopeRuleContract> PlaceholderRuleContracts()
	{
		TArray<FJungleRidgeSlopeRuleContract> Rules;

		FJungleRidgeSlopeRuleContract RidgeSkyWindow;
		RidgeSkyWindow.Zone = EJungleRidgeSlopeMountainZone::RidgeCrestSkyWindow;
		RidgeSkyWindow.EvidenceCue = EJungleRidgeSlopeEvidenceCue::SkyWindow;
		RidgeSkyWindow.RuleKind = EJunglePCGSpawnRuleKind::SkyWindowPreservationZone;
		RidgeSkyWindow.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		RidgeSkyWindow.RequiredMaskNames = { TEXT("ridge_valley"), TEXT("sky_window"), TEXT("canopy") };
		RidgeSkyWindow.MaxInstancesPerHectare = 12.0f;
		RidgeSkyWindow.MinimumSpacingMeters = 20.0f;
		RidgeSkyWindow.bPreservesSkyWindow = true;
		RidgeSkyWindow.bCanAnchorSilhouette = true;
		RidgeSkyWindow.bRequiresFutureHLODReview = true;
		Rules.Add(RidgeSkyWindow);

		FJungleRidgeSlopeRuleContract SpurAffordance;
		SpurAffordance.Zone = EJungleRidgeSlopeMountainZone::SpurMovementAffordance;
		SpurAffordance.EvidenceCue = EJungleRidgeSlopeEvidenceCue::SpurLineMovement;
		SpurAffordance.RuleKind = EJunglePCGSpawnRuleKind::AffordanceCuePlaceholder;
		SpurAffordance.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		SpurAffordance.RequiredMaskNames = { TEXT("ridge_valley"), TEXT("affordance_zone"), TEXT("slope") };
		SpurAffordance.MaxInstancesPerHectare = 18.0f;
		SpurAffordance.MinimumSpacingMeters = 16.0f;
		Rules.Add(SpurAffordance);

		FJungleRidgeSlopeRuleContract SlopeRisk;
		SlopeRisk.Zone = EJungleRidgeSlopeMountainZone::SteepSlopeFatigueRisk;
		SlopeRisk.EvidenceCue = EJungleRidgeSlopeEvidenceCue::FatigueRouteReconsideration;
		SlopeRisk.RuleKind = EJunglePCGSpawnRuleKind::RockRootPlaceholder;
		SlopeRisk.FrictionClass = EJunglePCGTerrainFrictionClass::ReadablePlantFriction;
		SlopeRisk.RequiredMaskNames = { TEXT("slope"), TEXT("footing_risk"), TEXT("soft_blocker") };
		SlopeRisk.MaxInstancesPerHectare = 34.0f;
		SlopeRisk.MinimumSpacingMeters = 8.0f;
		Rules.Add(SlopeRisk);

		FJungleRidgeSlopeRuleContract FalseSkyline;
		FalseSkyline.Zone = EJungleRidgeSlopeMountainZone::FalseSkylineAffordance;
		FalseSkyline.EvidenceCue = EJungleRidgeSlopeEvidenceCue::FalseOpenRidgeLine;
		FalseSkyline.RuleKind = EJunglePCGSpawnRuleKind::FalseAffordanceCuePlaceholder;
		FalseSkyline.FrictionClass = EJunglePCGTerrainFrictionClass::FalseAffordance;
		FalseSkyline.RequiredMaskNames = { TEXT("false_affordance"), TEXT("sky_window"), TEXT("canopy") };
		FalseSkyline.MaxInstancesPerHectare = 8.0f;
		FalseSkyline.MinimumSpacingMeters = 24.0f;
		FalseSkyline.bPreservesSkyWindow = false;
		Rules.Add(FalseSkyline);

		return Rules;
	}

	static TArray<FString> RequiredValidationOutputNames()
	{
		return {
			TEXT("ridge_slope_zone"),
			TEXT("ridge_sky_window"),
			TEXT("spur_affordance"),
			TEXT("saddle_transition"),
			TEXT("slope_break_decision"),
			TEXT("mountain_silhouette_anchor"),
			TEXT("steep_slope_fatigue_risk"),
			TEXT("orientation_cue_floor"),
			TEXT("weather_degradation_state"),
			TEXT("canopy_sky_window_profile"),
		};
	}
};
