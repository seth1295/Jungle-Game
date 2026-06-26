#pragma once

#include "CoreMinimal.h"

struct FJGTerrainSample
{
	float HeightM = 0.0f;
	float BaseHeightM = 0.0f;
	float RadiusM = 0.0f;
	float OrganicIslandRadiusM = 0.0f;
	float NormalizedRadius = 0.0f;
	float SignedShorelineDistanceM = 0.0f;
	float OceanMask = 0.0f;
	float CoastalShelfMask = 0.0f;
	float BeachRingMask = 0.0f;
	float CoastalLowlandMask = 0.0f;
	float MassifMask = 0.0f;
	float MassifHeightM = 0.0f;
	float RidgeMask = 0.0f;
	float GullyMask = 0.0f;
	float LaharCorridorMask = 0.0f;
	float CoastalFanMask = 0.0f;
	float CraterMask = 0.0f;
	float RimMask = 0.0f;
	float VentMask = 0.0f;
	float BreachMask = 0.0f;
	float LavaCrustMask = 0.0f;
	float UnstableCrustMask = 0.0f;
	float HardBlockerMask = 0.0f;
	float RidgeHeightM = 0.0f;
	float GullyIncisionM = 0.0f;
	float CraterDepressionM = 0.0f;
	int32 CatchmentId = INDEX_NONE;
};

struct FJGTerrainMetrics
{
	float MinHeightM = TNumericLimits<float>::Max();
	float MaxHeightM = TNumericLimits<float>::Lowest();
	float MinSquareEdgeHeightM = TNumericLimits<float>::Max();
	float MaxSquareEdgeHeightM = TNumericLimits<float>::Lowest();
	float MinIslandRadiusM = TNumericLimits<float>::Max();
	float MaxIslandRadiusM = TNumericLimits<float>::Lowest();
	float MinOceanMarginM = TNumericLimits<float>::Max();
	float MaxRidgeMask = 0.0f;
	float MaxGullyMask = 0.0f;
	float MaxLaharCorridorMask = 0.0f;
	float MaxCraterMask = 0.0f;
	float MaxVentMask = 0.0f;
	float MaxHardBlockerMask = 0.0f;
	int32 SampleCount = 0;
	int32 CatchmentCount = 14;
	int32 OceanEdgeSampleCount = 0;
};

class JUNGLEGAME_API FJungleVolcanicIslandTerrainModel
{
public:
	static constexpr float WorldSizeM = 16256.0f;
	static constexpr float HalfExtentM = WorldSizeM * 0.5f;
	static constexpr float SeaLevelM = 0.0f;
	static constexpr float MeanIslandRadiusM = 7000.0f;
	static constexpr float MaxIslandRadiusM = 7350.0f;
	static constexpr float TargetPeakHeightM = 1400.0f;
	static constexpr int32 PrimaryCatchmentCount = 14;
	static constexpr int32 RuntimePreviewVerticesPerSide = 129;

	static FJGTerrainSample SampleTerrainMeters(float WorldXM, float WorldYM);
	static float SampleHeightMeters(float WorldXM, float WorldYM);
	static float OrganicIslandRadiusMeters(float ThetaRadians);
	static FJGTerrainMetrics BuildMetrics(int32 SamplesPerSide = RuntimePreviewVerticesPerSide);
	static FString BuildMetricsLogLine(const FJGTerrainMetrics& Metrics);

private:
	static float SmoothStep(float Edge0, float Edge1, float Value);
	static float RingMask(float DistanceM, float CenterM, float HalfWidthM);
};
