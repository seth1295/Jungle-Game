#pragma once

#include "CoreMinimal.h"

/** Shared macro landform vocabulary for the first readable island terrain pass. */
enum class EJungleMacroTerrainClass : uint8
{
	OceanBoundary,
	Beach,
	MangroveEdge,
	CliffCoast,
	RockyHeadland,
	CreekMouth,
	MountainSpine,
	Ridge,
	Spur,
	Saddle,
	SlopeBreak,
	Valley,
	Gully,
	CreekChannel,
	Basin,
	FalseAffordance,
	SoftBlocker,
	HardBlocker,
	AffordanceZone,
};

struct FJungleMacroTerrainSpec
{
	static constexpr double MaximumRoutineWalkableSlopeDegrees = 32.0;
	static constexpr double SoftBlockerSlopeDegrees = 38.0;
	static constexpr double HardBlockerSlopeDegrees = 48.0;
	static constexpr double MinimumCreekCatchmentHectares = 2.0;
	static constexpr double MinimumReadableRidgeProminenceMeters = 18.0;

	static bool IsSlopeHardBlocker(double SlopeDegrees)
	{
		return SlopeDegrees >= HardBlockerSlopeDegrees;
	}

	static bool IsSlopeSoftBlocker(double SlopeDegrees)
	{
		return SlopeDegrees >= SoftBlockerSlopeDegrees && SlopeDegrees < HardBlockerSlopeDegrees;
	}

	static bool IsRoutineWalkableSlope(double SlopeDegrees)
	{
		return SlopeDegrees <= MaximumRoutineWalkableSlopeDegrees;
	}

	static bool HasReadableRidgeProminence(double ProminenceMeters)
	{
		return ProminenceMeters >= MinimumReadableRidgeProminenceMeters;
	}
};
