#pragma once

#include "CoreMinimal.h"

/** Terrain-derived ecosystem and traversal mask identifiers. */
enum class EJungleEcosystemMask : uint8
{
	Height,
	Slope,
	Aspect,
	CreekDistance,
	CoastDistance,
	Wetness,
	RidgeValley,
	LandZone,
	Canopy,
	Disturbance,
	SoundPropagation,
	Visibility,
	SkyWindow,
	FootingRisk,
	EcosystemTell,
	NightDanger,
	HardBlocker,
	SoftBlocker,
	AffordanceZone,
	FalseAffordance,
};

/** Canonical traversal categories consumed by future ecosystem and debug passes. */
enum class EJungleTraversalMaskClass : uint8
{
	Open,
	Affordance,
	SoftBlocker,
	HardBlocker,
	FalseAffordance,
};

struct FJungleEcosystemMaskSpec
{
	static constexpr float NormalizedMin = 0.0f;
	static constexpr float NormalizedMax = 1.0f;
	static constexpr float HardBlockerThreshold = 0.85f;
	static constexpr float SoftBlockerThreshold = 0.55f;
	static constexpr float AffordanceThreshold = 0.65f;
	static constexpr float FalseAffordanceThreshold = 0.70f;

	static bool IsNormalized(float Value)
	{
		return Value >= NormalizedMin && Value <= NormalizedMax;
	}

	static EJungleTraversalMaskClass ClassifyTraversal(float HardBlocker, float SoftBlocker, float Affordance, float FalseAffordance)
	{
		if (HardBlocker >= HardBlockerThreshold)
		{
			return EJungleTraversalMaskClass::HardBlocker;
		}

		if (FalseAffordance >= FalseAffordanceThreshold)
		{
			return EJungleTraversalMaskClass::FalseAffordance;
		}

		if (SoftBlocker >= SoftBlockerThreshold)
		{
			return EJungleTraversalMaskClass::SoftBlocker;
		}

		if (Affordance >= AffordanceThreshold)
		{
			return EJungleTraversalMaskClass::Affordance;
		}

		return EJungleTraversalMaskClass::Open;
	}

	static TArray<FString> RequiredMaskNames()
	{
		return {
			TEXT("height"),
			TEXT("slope"),
			TEXT("aspect"),
			TEXT("creek_distance"),
			TEXT("coast_distance"),
			TEXT("wetness"),
			TEXT("ridge_valley"),
			TEXT("land_zone"),
			TEXT("canopy"),
			TEXT("disturbance"),
			TEXT("sound_propagation"),
			TEXT("visibility"),
			TEXT("sky_window"),
			TEXT("footing_risk"),
			TEXT("ecosystem_tell"),
			TEXT("night_danger"),
			TEXT("hard_blocker"),
			TEXT("soft_blocker"),
			TEXT("affordance_zone"),
			TEXT("false_affordance"),
		};
	}
};
