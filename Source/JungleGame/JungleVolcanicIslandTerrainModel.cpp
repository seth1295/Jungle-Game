#include "JungleVolcanicIslandTerrainModel.h"

float FJungleVolcanicIslandTerrainModel::SmoothStep(float Edge0, float Edge1, float Value)
{
	if (FMath::IsNearlyEqual(Edge0, Edge1))
	{
		return Value >= Edge1 ? 1.0f : 0.0f;
	}

	const float T = FMath::Clamp((Value - Edge0) / (Edge1 - Edge0), 0.0f, 1.0f);
	return T * T * (3.0f - 2.0f * T);
}

float FJungleVolcanicIslandTerrainModel::RingMask(float DistanceM, float CenterM, float HalfWidthM)
{
	const float Delta = FMath::Abs(DistanceM - CenterM);
	return 1.0f - SmoothStep(HalfWidthM * 0.35f, HalfWidthM, Delta);
}

float FJungleVolcanicIslandTerrainModel::OrganicIslandRadiusMeters(float ThetaRadians)
{
	const float BroadLobes =
		260.0f * FMath::Sin(3.0f * ThetaRadians + 0.70f) +
		170.0f * FMath::Sin(5.0f * ThetaRadians - 1.10f) +
		75.0f * FMath::Sin(9.0f * ThetaRadians + 2.30f);

	const float BayHeadlandWarp =
		95.0f * FMath::Sin(2.0f * ThetaRadians - 0.35f) *
		FMath::Sin(7.0f * ThetaRadians + 1.60f);

	return FMath::Clamp(MeanIslandRadiusM + BroadLobes + BayHeadlandWarp, 6200.0f, MaxIslandRadiusM);
}

FJGTerrainSample FJungleVolcanicIslandTerrainModel::SampleTerrainMeters(float WorldXM, float WorldYM)
{
	FJGTerrainSample Sample;

	const FVector2D WorldXY(WorldXM, WorldYM);
	const float RadiusM = WorldXY.Size();
	const float Theta = FMath::Atan2(WorldYM, WorldXM);
	const float IslandRadiusM = OrganicIslandRadiusMeters(Theta);
	const float SignedShorelineDistanceM = IslandRadiusM - RadiusM;
	const float NormalizedRadius = IslandRadiusM > 0.0f ? RadiusM / IslandRadiusM : 0.0f;
	const float LandMask = SmoothStep(-10.0f, 60.0f, SignedShorelineDistanceM);

	const float SeawardDistanceM = FMath::Max(0.0f, -SignedShorelineDistanceM);
	const float LandwardDistanceM = FMath::Max(0.0f, SignedShorelineDistanceM);

	const float OuterOceanDepthM = FMath::Lerp(-12.0f, -145.0f, SmoothStep(250.0f, 1000.0f, SeawardDistanceM));
	const float CoastalShelfHeightM = FMath::Lerp(-2.0f, -26.0f, SmoothStep(0.0f, 700.0f, SeawardDistanceM));
	const float OceanHeightM = SeawardDistanceM < 700.0f ? CoastalShelfHeightM : OuterOceanDepthM;

	float CoastalLandHeightM = 0.0f;
	if (LandwardDistanceM <= 90.0f)
	{
		CoastalLandHeightM = FMath::Lerp(0.0f, 3.5f, SmoothStep(0.0f, 90.0f, LandwardDistanceM));
	}
	else if (LandwardDistanceM <= 240.0f)
	{
		CoastalLandHeightM = FMath::Lerp(3.5f, 9.0f, SmoothStep(90.0f, 240.0f, LandwardDistanceM));
	}
	else if (LandwardDistanceM <= 1100.0f)
	{
		CoastalLandHeightM = FMath::Lerp(9.0f, 62.0f, SmoothStep(240.0f, 1100.0f, LandwardDistanceM));
	}
	else
	{
		CoastalLandHeightM = FMath::Lerp(62.0f, 155.0f, SmoothStep(1100.0f, 2300.0f, LandwardDistanceM));
	}

	const FVector2D MassifCenterM(-220.0f, 180.0f);
	const FVector2D FromMassifM = WorldXY - MassifCenterM;
	const float MassifDistanceM = FromMassifM.Size();
	const float MassifTheta = FMath::Atan2(FromMassifM.Y, FromMassifM.X);
	const float MassifBaseRadiusM = 4350.0f;
	const float MassifNormalized = FMath::Clamp(MassifDistanceM / MassifBaseRadiusM, 0.0f, 1.3f);
	const float ConcaveShield = FMath::Pow(FMath::Clamp(1.0f - FMath::Pow(MassifNormalized, 1.55f), 0.0f, 1.0f), 1.18f);
	const float UpperSteepening = FMath::Pow(FMath::Clamp(1.0f - MassifDistanceM / 1650.0f, 0.0f, 1.0f), 2.2f) * 165.0f;
	const float ShoulderBench = FMath::Exp(-FMath::Square((MassifDistanceM - 2850.0f) / 520.0f)) * 55.0f;
	const float Asymmetry =
		FMath::Sin(WorldXM * 0.00042f + 0.9f) * 24.0f +
		FMath::Sin(WorldYM * 0.00036f - 1.7f) * 18.0f;
	const float CoastalProtection = SmoothStep(1250.0f, 2400.0f, LandwardDistanceM) * LandMask;
	const float MassifMask = FMath::Clamp(SmoothStep(0.04f, 0.82f, 1.0f - MassifNormalized) * CoastalProtection, 0.0f, 1.0f);
	const float MassifHeightM = (TargetPeakHeightM * ConcaveShield + UpperSteepening + ShoulderBench + Asymmetry) * CoastalProtection;

	const float NormalizedCatchmentFloat = (MassifTheta + PI) / (2.0f * PI) * static_cast<float>(PrimaryCatchmentCount);
	const float WrappedCatchmentFloat = FMath::Fmod(NormalizedCatchmentFloat + static_cast<float>(PrimaryCatchmentCount), static_cast<float>(PrimaryCatchmentCount));
	const int32 CatchmentId = FMath::Clamp(FMath::FloorToInt(WrappedCatchmentFloat), 0, PrimaryCatchmentCount - 1);
	const int32 NearestRidgeIndex = FMath::RoundToInt(WrappedCatchmentFloat) % PrimaryCatchmentCount;
	const float RidgeTheta = (static_cast<float>(NearestRidgeIndex) / static_cast<float>(PrimaryCatchmentCount)) * 2.0f * PI - PI;
	const float GullyTheta = ((static_cast<float>(CatchmentId) + 0.5f) / static_cast<float>(PrimaryCatchmentCount)) * 2.0f * PI - PI;
	const float RidgeAngularDelta = FMath::Abs(FMath::Atan2(FMath::Sin(MassifTheta - RidgeTheta), FMath::Cos(MassifTheta - RidgeTheta)));
	const float GullyAngularDelta = FMath::Abs(FMath::Atan2(FMath::Sin(MassifTheta - GullyTheta), FMath::Cos(MassifTheta - GullyTheta)));
	const float MidSlopeMask = SmoothStep(1450.0f, 2750.0f, MassifDistanceM) * (1.0f - SmoothStep(5600.0f, 6600.0f, MassifDistanceM)) * LandMask;
	const float RidgeMask = (1.0f - SmoothStep(0.035f, 0.125f, RidgeAngularDelta)) * MidSlopeMask * SmoothStep(1350.0f, 2200.0f, LandwardDistanceM);
	const float GullyReachMask = SmoothStep(1800.0f, 2900.0f, MassifDistanceM) * SmoothStep(700.0f, 1350.0f, LandwardDistanceM) * LandMask;
	const float GullyMask = (1.0f - SmoothStep(0.038f, 0.120f, GullyAngularDelta)) * GullyReachMask;
	const bool bLaharCatchment = CatchmentId == 1 || CatchmentId == 4 || CatchmentId == 7 || CatchmentId == 10 || CatchmentId == 12;
	const float LaharCorridorMask = bLaharCatchment ? GullyMask * SmoothStep(2600.0f, 4300.0f, MassifDistanceM) : 0.0f;
	const float CoastalFanMask = (1.0f - SmoothStep(0.045f, 0.150f, GullyAngularDelta)) * SmoothStep(260.0f, 720.0f, LandwardDistanceM) * (1.0f - SmoothStep(1050.0f, 1650.0f, LandwardDistanceM)) * LandMask;
	const float RidgeHeightM = RidgeMask * FMath::Lerp(28.0f, 96.0f, SmoothStep(2600.0f, 4300.0f, MassifDistanceM));
	const float GullyIncisionM = GullyMask * (FMath::Lerp(18.0f, 72.0f, SmoothStep(2200.0f, 5100.0f, MassifDistanceM)) + LaharCorridorMask * 46.0f);
	const float FanDepositM = CoastalFanMask * FMath::Lerp(8.0f, 26.0f, SmoothStep(380.0f, 900.0f, LandwardDistanceM));

	const float LongWaveUndulationM =
		FMath::Sin(WorldXM * 0.0018f + WorldYM * 0.0011f) * 8.0f +
		FMath::Sin(WorldXM * 0.0027f - WorldYM * 0.0016f) * 5.0f;

	const float TerrainProcessHeightM = MassifHeightM + LongWaveUndulationM * LandMask + RidgeHeightM - GullyIncisionM + FanDepositM;
	const float LandHeightM = FMath::Max(CoastalLandHeightM - GullyIncisionM * 0.18f, CoastalLandHeightM + TerrainProcessHeightM);

	Sample.HeightM = FMath::Lerp(OceanHeightM, LandHeightM, LandMask);
	Sample.BaseHeightM = FMath::Lerp(OceanHeightM, CoastalLandHeightM, LandMask);
	Sample.RadiusM = RadiusM;
	Sample.OrganicIslandRadiusM = IslandRadiusM;
	Sample.NormalizedRadius = NormalizedRadius;
	Sample.SignedShorelineDistanceM = SignedShorelineDistanceM;
	Sample.OceanMask = 1.0f - LandMask;
	Sample.CoastalShelfMask = RingMask(SignedShorelineDistanceM, -350.0f, 420.0f);
	Sample.BeachRingMask = RingMask(SignedShorelineDistanceM, 55.0f, 145.0f);
	Sample.CoastalLowlandMask = FMath::Clamp(SmoothStep(180.0f, 1200.0f, LandwardDistanceM) * (1.0f - SmoothStep(1250.0f, 2350.0f, LandwardDistanceM)) * LandMask, 0.0f, 1.0f);
	Sample.MassifMask = MassifMask;
	Sample.MassifHeightM = MassifHeightM;
	Sample.RidgeMask = RidgeMask;
	Sample.GullyMask = GullyMask;
	Sample.LaharCorridorMask = LaharCorridorMask;
	Sample.CoastalFanMask = CoastalFanMask;
	Sample.RidgeHeightM = RidgeHeightM;
	Sample.GullyIncisionM = GullyIncisionM;
	Sample.CatchmentId = LandMask > 0.5f ? CatchmentId : INDEX_NONE;

	return Sample;
}

float FJungleVolcanicIslandTerrainModel::SampleHeightMeters(float WorldXM, float WorldYM)
{
	return SampleTerrainMeters(WorldXM, WorldYM).HeightM;
}

FJGTerrainMetrics FJungleVolcanicIslandTerrainModel::BuildMetrics(int32 SamplesPerSide)
{
	FJGTerrainMetrics Metrics;
	Metrics.CatchmentCount = PrimaryCatchmentCount;
	const int32 ClampedSamplesPerSide = FMath::Max(2, SamplesPerSide);
	const float StepM = WorldSizeM / static_cast<float>(ClampedSamplesPerSide - 1);

	for (int32 Y = 0; Y < ClampedSamplesPerSide; ++Y)
	{
		for (int32 X = 0; X < ClampedSamplesPerSide; ++X)
		{
			const float WorldXM = -HalfExtentM + static_cast<float>(X) * StepM;
			const float WorldYM = -HalfExtentM + static_cast<float>(Y) * StepM;
			const FJGTerrainSample Sample = SampleTerrainMeters(WorldXM, WorldYM);

			Metrics.MinHeightM = FMath::Min(Metrics.MinHeightM, Sample.HeightM);
			Metrics.MaxHeightM = FMath::Max(Metrics.MaxHeightM, Sample.HeightM);
			Metrics.MinIslandRadiusM = FMath::Min(Metrics.MinIslandRadiusM, Sample.OrganicIslandRadiusM);
			Metrics.MaxIslandRadiusM = FMath::Max(Metrics.MaxIslandRadiusM, Sample.OrganicIslandRadiusM);
			Metrics.MinOceanMarginM = FMath::Min(Metrics.MinOceanMarginM, HalfExtentM - Sample.OrganicIslandRadiusM);
			Metrics.MaxRidgeMask = FMath::Max(Metrics.MaxRidgeMask, Sample.RidgeMask);
			Metrics.MaxGullyMask = FMath::Max(Metrics.MaxGullyMask, Sample.GullyMask);
			Metrics.MaxLaharCorridorMask = FMath::Max(Metrics.MaxLaharCorridorMask, Sample.LaharCorridorMask);
			++Metrics.SampleCount;

			const bool bSquareEdgeSample = X == 0 || Y == 0 || X == ClampedSamplesPerSide - 1 || Y == ClampedSamplesPerSide - 1;
			if (bSquareEdgeSample)
			{
				Metrics.MinSquareEdgeHeightM = FMath::Min(Metrics.MinSquareEdgeHeightM, Sample.HeightM);
				Metrics.MaxSquareEdgeHeightM = FMath::Max(Metrics.MaxSquareEdgeHeightM, Sample.HeightM);
				++Metrics.OceanEdgeSampleCount;
			}
		}
	}

	return Metrics;
}

FString FJungleVolcanicIslandTerrainModel::BuildMetricsLogLine(const FJGTerrainMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=JG_VOLCANIC_TERRAIN_003 world_m=%.0f sea_level_m=%.1f samples=%d height_min_m=%.1f height_max_m=%.1f island_radius_min_m=%.1f island_radius_max_m=%.1f ocean_margin_min_m=%.1f square_edge_height_min_m=%.1f square_edge_height_max_m=%.1f target_peak_m=%.1f catchments=%d ridge_mask_max=%.2f gully_mask_max=%.2f lahar_mask_max=%.2f"),
		WorldSizeM,
		SeaLevelM,
		Metrics.SampleCount,
		Metrics.MinHeightM,
		Metrics.MaxHeightM,
		Metrics.MinIslandRadiusM,
		Metrics.MaxIslandRadiusM,
		Metrics.MinOceanMarginM,
		Metrics.MinSquareEdgeHeightM,
		Metrics.MaxSquareEdgeHeightM,
		TargetPeakHeightM,
		Metrics.CatchmentCount,
		Metrics.MaxRidgeMask,
		Metrics.MaxGullyMask,
		Metrics.MaxLaharCorridorMask);
}
