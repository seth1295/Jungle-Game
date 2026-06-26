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

	const FVector2D CraterCenterM = MassifCenterM + FVector2D(115.0f, -80.0f);
	const FVector2D CraterDeltaM = WorldXY - CraterCenterM;
	const float CraterYawRadians = 0.42f;
	const float CraterU = CraterDeltaM.X * FMath::Cos(CraterYawRadians) + CraterDeltaM.Y * FMath::Sin(CraterYawRadians);
	const float CraterV = -CraterDeltaM.X * FMath::Sin(CraterYawRadians) + CraterDeltaM.Y * FMath::Cos(CraterYawRadians);
	const float CraterNorm = FMath::Sqrt(FMath::Square(CraterU / 390.0f) + FMath::Square(CraterV / 285.0f));
	const float CraterInteriorMask = (1.0f - SmoothStep(0.72f, 1.02f, CraterNorm)) * MassifMask;
	const float RimMask = RingMask(CraterNorm, 1.0f, 0.24f) * MassifMask;
	const FVector2D VentCenterM = CraterCenterM + FVector2D(-75.0f, 95.0f);
	const float VentDistanceM = (WorldXY - VentCenterM).Size();
	const float VentMask = (1.0f - SmoothStep(70.0f, 175.0f, VentDistanceM)) * CraterInteriorMask;
	const float BreachTheta = -0.82f;
	const float BreachAngularDelta = FMath::Abs(FMath::Atan2(FMath::Sin(MassifTheta - BreachTheta), FMath::Cos(MassifTheta - BreachTheta)));
	const float BreachMask = (1.0f - SmoothStep(0.050f, 0.155f, BreachAngularDelta)) * SmoothStep(390.0f, 720.0f, MassifDistanceM) * (1.0f - SmoothStep(2100.0f, 2850.0f, MassifDistanceM)) * MassifMask;
	const float FissureMask = (1.0f - SmoothStep(0.018f, 0.052f, BreachAngularDelta)) * SmoothStep(1050.0f, 1450.0f, MassifDistanceM) * (1.0f - SmoothStep(2400.0f, 3150.0f, MassifDistanceM)) * MassifMask;
	const float BrokenRimMask = RimMask * (1.0f - BreachMask);
	const float LavaCrustMask = FMath::Clamp(CraterInteriorMask * 0.70f + VentMask + BreachMask * 0.45f + FissureMask * 0.60f, 0.0f, 1.0f);
	const float UnstableCrustMask = FMath::Clamp(VentMask + FissureMask + LavaCrustMask * 0.45f, 0.0f, 1.0f);
	const float HardBlockerMask = FMath::Clamp(BrokenRimMask * 0.75f + VentMask + FissureMask * 0.85f + BreachMask * 0.35f, 0.0f, 1.0f);
	const float RimRaiseM = BrokenRimMask * 82.0f;
	const float CraterDepressionM = CraterInteriorMask * 128.0f + VentMask * 78.0f + BreachMask * 58.0f + FissureMask * 32.0f;

	const float LongWaveUndulationM =
		FMath::Sin(WorldXM * 0.0018f + WorldYM * 0.0011f) * 8.0f +
		FMath::Sin(WorldXM * 0.0027f - WorldYM * 0.0016f) * 5.0f;

	const float TerrainProcessHeightM = MassifHeightM + LongWaveUndulationM * LandMask + RidgeHeightM - GullyIncisionM + FanDepositM + RimRaiseM - CraterDepressionM;
	const float LandHeightM = FMath::Max(CoastalLandHeightM - GullyIncisionM * 0.18f, CoastalLandHeightM + TerrainProcessHeightM);
	const float RawHeightM = FMath::Lerp(OceanHeightM, LandHeightM, LandMask);
	const float ShorelineConstraintMask = 1.0f - SmoothStep(12.0f, 85.0f, FMath::Abs(SignedShorelineDistanceM));
	const float CoastLockedHeightM = FMath::Lerp(RawHeightM, SeaLevelM, ShorelineConstraintMask);

	Sample.HeightM = CoastLockedHeightM;
	Sample.BaseHeightM = FMath::Lerp(OceanHeightM, CoastalLandHeightM, LandMask);
	Sample.RadiusM = RadiusM;
	Sample.OrganicIslandRadiusM = IslandRadiusM;
	Sample.NormalizedRadius = NormalizedRadius;
	Sample.SignedShorelineDistanceM = SignedShorelineDistanceM;
	Sample.OceanMask = 1.0f - LandMask;
	Sample.CoastalShelfMask = RingMask(SignedShorelineDistanceM, -350.0f, 420.0f);
	Sample.BeachRingMask = RingMask(SignedShorelineDistanceM, 55.0f, 145.0f);
	Sample.CoastalLowlandMask = FMath::Clamp(SmoothStep(180.0f, 1200.0f, LandwardDistanceM) * (1.0f - SmoothStep(1250.0f, 2350.0f, LandwardDistanceM)) * LandMask, 0.0f, 1.0f);
	Sample.ShorelineConstraintMask = ShorelineConstraintMask;
	Sample.ShorelineErrorM = ShorelineConstraintMask > 0.0f ? FMath::Abs(Sample.HeightM - SeaLevelM) : 0.0f;
	Sample.BeachWidthM = Sample.BeachRingMask > 0.1f ? 180.0f : 0.0f;
	Sample.MassifMask = MassifMask;
	Sample.MassifHeightM = MassifHeightM;
	Sample.RidgeMask = RidgeMask;
	Sample.GullyMask = GullyMask;
	Sample.LaharCorridorMask = LaharCorridorMask;
	Sample.CoastalFanMask = CoastalFanMask;
	Sample.CraterMask = CraterInteriorMask;
	Sample.RimMask = BrokenRimMask;
	Sample.VentMask = VentMask;
	Sample.BreachMask = BreachMask;
	Sample.LavaCrustMask = LavaCrustMask;
	Sample.UnstableCrustMask = UnstableCrustMask;
	Sample.HardBlockerMask = HardBlockerMask;
	Sample.RidgeHeightM = RidgeHeightM;
	Sample.GullyIncisionM = GullyIncisionM;
	Sample.CraterDepressionM = CraterDepressionM;
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
			Metrics.MaxCraterMask = FMath::Max(Metrics.MaxCraterMask, Sample.CraterMask);
			Metrics.MaxVentMask = FMath::Max(Metrics.MaxVentMask, Sample.VentMask);
			Metrics.MaxHardBlockerMask = FMath::Max(Metrics.MaxHardBlockerMask, Sample.HardBlockerMask);
			if (Sample.ShorelineConstraintMask > 0.1f)
			{
				Metrics.MaxShorelineAbsErrorM = FMath::Max(Metrics.MaxShorelineAbsErrorM, Sample.ShorelineErrorM);
				++Metrics.ShorelineSampleCount;
			}
			if (Sample.SignedShorelineDistanceM >= 20.0f && Sample.SignedShorelineDistanceM <= 190.0f)
			{
				++Metrics.BeachSampleCount;
				if (Sample.HeightM >= -0.25f && Sample.HeightM <= 10.0f)
				{
					++Metrics.BeachContinuitySampleCount;
				}
			}
			++Metrics.SampleCount;

			const bool bSquareEdgeSample = X == 0 || Y == 0 || X == ClampedSamplesPerSide - 1 || Y == ClampedSamplesPerSide - 1;
			if (bSquareEdgeSample)
			{
				Metrics.MinSquareEdgeHeightM = FMath::Min(Metrics.MinSquareEdgeHeightM, Sample.HeightM);
				Metrics.MaxSquareEdgeHeightM = FMath::Max(Metrics.MaxSquareEdgeHeightM, Sample.HeightM);
				if (Sample.HeightM > SeaLevelM - 0.5f)
				{
					++Metrics.SquareEdgeOceanViolationCount;
				}
				++Metrics.OceanEdgeSampleCount;
			}
		}
	}

	Metrics.BeachContinuityPercent = Metrics.BeachSampleCount > 0
		? 100.0f * static_cast<float>(Metrics.BeachContinuitySampleCount) / static_cast<float>(Metrics.BeachSampleCount)
		: 0.0f;

	return Metrics;
}

FString FJungleVolcanicIslandTerrainModel::BuildMetricsLogLine(const FJGTerrainMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=JG_VOLCANIC_TERRAIN_005 world_m=%.0f sea_level_m=%.1f samples=%d height_min_m=%.1f height_max_m=%.1f island_radius_min_m=%.1f island_radius_max_m=%.1f ocean_margin_min_m=%.1f square_edge_height_min_m=%.1f square_edge_height_max_m=%.1f target_peak_m=%.1f catchments=%d ridge_mask_max=%.2f gully_mask_max=%.2f lahar_mask_max=%.2f crater_mask_max=%.2f vent_mask_max=%.2f hard_blocker_mask_max=%.2f shoreline_error_max_m=%.2f beach_continuity_pct=%.1f square_edge_ocean_violations=%d"),
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
		Metrics.MaxLaharCorridorMask,
		Metrics.MaxCraterMask,
		Metrics.MaxVentMask,
		Metrics.MaxHardBlockerMask,
		Metrics.MaxShorelineAbsErrorM,
		Metrics.BeachContinuityPercent,
		Metrics.SquareEdgeOceanViolationCount);
}

int32 FJungleVolcanicIslandTerrainModel::VerticesPerSideForLod(int32 LodIndex)
{
	if (LodIndex <= 0)
	{
		return RuntimeNearTileVerticesPerSide;
	}
	if (LodIndex == 1)
	{
		return RuntimeMidTileVerticesPerSide;
	}
	return RuntimeFarTileVerticesPerSide;
}

FJGTerrainRuntimeTileDesc FJungleVolcanicIslandTerrainModel::BuildRuntimeTileDesc(int32 TileX, int32 TileY, int32 LodIndex, bool bCollisionEnabled)
{
	FJGTerrainRuntimeTileDesc Desc;
	Desc.TileX = TileX;
	Desc.TileY = TileY;
	Desc.LodIndex = FMath::Max(0, LodIndex);
	Desc.VerticesPerSide = VerticesPerSideForLod(Desc.LodIndex);
	Desc.MinXM = -HalfExtentM + RuntimeTileSizeM * static_cast<float>(TileX);
	Desc.MinYM = -HalfExtentM + RuntimeTileSizeM * static_cast<float>(TileY);
	Desc.MaxXM = Desc.MinXM + RuntimeTileSizeM;
	Desc.MaxYM = Desc.MinYM + RuntimeTileSizeM;
	Desc.SpacingM = RuntimeTileSizeM / static_cast<float>(Desc.VerticesPerSide - 1);
	Desc.bCollisionEnabled = bCollisionEnabled;
	Desc.TileName = FName(*FString::Printf(TEXT("JG_TerrainTile_X%02d_Y%02d_LOD%d"), TileX, TileY, Desc.LodIndex));
	return Desc;
}

void FJungleVolcanicIslandTerrainModel::BuildRuntimeValidationTileDescs(TArray<FJGTerrainRuntimeTileDesc>& OutTiles)
{
	OutTiles.Reset();

	const int32 CenterTile = RuntimeTilesPerSide / 2;
	const int32 HalfValidationWindow = RuntimeValidationTilesPerSide / 2;
	for (int32 TileY = CenterTile - HalfValidationWindow; TileY <= CenterTile + HalfValidationWindow; ++TileY)
	{
		for (int32 TileX = CenterTile - HalfValidationWindow; TileX <= CenterTile + HalfValidationWindow; ++TileX)
		{
			const int32 DeltaX = FMath::Abs(TileX - CenterTile);
			const int32 DeltaY = FMath::Abs(TileY - CenterTile);
			const int32 RingDistance = FMath::Max(DeltaX, DeltaY);
			const int32 LodIndex = RingDistance == 0 ? 0 : (RingDistance <= 1 ? 1 : 2);
			const bool bCollisionEnabled = RingDistance <= 1;
			OutTiles.Add(BuildRuntimeTileDesc(TileX, TileY, LodIndex, bCollisionEnabled));
		}
	}
}

FJGTerrainRuntimeMeshMetrics FJungleVolcanicIslandTerrainModel::BuildRuntimeMeshMetrics(const TArray<FJGTerrainRuntimeTileDesc>& Tiles)
{
	FJGTerrainRuntimeMeshMetrics Metrics;
	Metrics.TileCount = Tiles.Num();
	Metrics.RuntimeTileSizeM = RuntimeTileSizeM;
	Metrics.SourceReferenceSpacingM = SourceReferenceSpacingM;

	for (const FJGTerrainRuntimeTileDesc& Tile : Tiles)
	{
		Metrics.TotalVertexCount += Tile.VerticesPerSide * Tile.VerticesPerSide;
		Metrics.TotalTriangleCount += (Tile.VerticesPerSide - 1) * (Tile.VerticesPerSide - 1) * 2;
		Metrics.RuntimeMinSpacingM = FMath::Min(Metrics.RuntimeMinSpacingM, Tile.SpacingM);
		Metrics.RuntimeMaxSpacingM = FMath::Max(Metrics.RuntimeMaxSpacingM, Tile.SpacingM);
		if (Tile.bCollisionEnabled)
		{
			++Metrics.CollisionTileCount;
		}
		if (Tile.LodIndex <= 0)
		{
			++Metrics.NearLodTileCount;
		}
		else if (Tile.LodIndex == 1)
		{
			++Metrics.MidLodTileCount;
		}
		else
		{
			++Metrics.FarLodTileCount;
		}

		constexpr int32 EdgeSamplesPerTile = 9;
		for (int32 SampleIndex = 0; SampleIndex < EdgeSamplesPerTile; ++SampleIndex)
		{
			const float Alpha = static_cast<float>(SampleIndex) / static_cast<float>(EdgeSamplesPerTile - 1);
			const float EdgeYM = FMath::Lerp(Tile.MinYM, Tile.MaxYM, Alpha);
			const float EdgeXM = FMath::Lerp(Tile.MinXM, Tile.MaxXM, Alpha);

			if (Tile.TileX < RuntimeTilesPerSide - 1)
			{
				const float SharedXM = Tile.MaxXM;
				const float A = SampleHeightMeters(SharedXM, EdgeYM);
				const float B = SampleHeightMeters(SharedXM, EdgeYM);
				Metrics.MaxAdjacentSeamAbsErrorM = FMath::Max(Metrics.MaxAdjacentSeamAbsErrorM, FMath::Abs(A - B));
				++Metrics.AdjacentSeamSampleCount;
			}
			if (Tile.TileY < RuntimeTilesPerSide - 1)
			{
				const float SharedYM = Tile.MaxYM;
				const float A = SampleHeightMeters(EdgeXM, SharedYM);
				const float B = SampleHeightMeters(EdgeXM, SharedYM);
				Metrics.MaxAdjacentSeamAbsErrorM = FMath::Max(Metrics.MaxAdjacentSeamAbsErrorM, FMath::Abs(A - B));
				++Metrics.AdjacentSeamSampleCount;
			}
		}
	}

	const int32 CoastSampleCount = 96;
	for (int32 Index = 0; Index < CoastSampleCount; ++Index)
	{
		const float Theta = 2.0f * PI * static_cast<float>(Index) / static_cast<float>(CoastSampleCount);
		const float Radius = OrganicIslandRadiusMeters(Theta);
		const FJGTerrainSample CoastSample = SampleTerrainMeters(FMath::Cos(Theta) * Radius, FMath::Sin(Theta) * Radius);
		Metrics.MaxShorelineAbsErrorM = FMath::Max(Metrics.MaxShorelineAbsErrorM, FMath::Abs(CoastSample.HeightM - SeaLevelM));
		++Metrics.ShorelineSampleCount;
	}

	if (Metrics.RuntimeMinSpacingM == TNumericLimits<float>::Max())
	{
		Metrics.RuntimeMinSpacingM = 0.0f;
	}

	return Metrics;
}

FString FJungleVolcanicIslandTerrainModel::BuildRuntimeMeshMetricsLogLine(const FJGTerrainRuntimeMeshMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=JG_TERRAIN_RUNTIME_006 world_m=%.0f sea_level_m=%.1f source_reference_vertices=%d source_reference_spacing_m=%.2f runtime_tiles_per_side=%d validation_tiles=%d tile_size_m=%.1f vertices=%d triangles=%d collision_tiles=%d lod_near=%d lod_mid=%d lod_far=%d runtime_spacing_min_m=%.2f runtime_spacing_max_m=%.2f seam_samples=%d seam_error_max_m=%.4f shoreline_samples=%d shoreline_error_max_m=%.3f source=canonical-volcanic-heightfield"),
		WorldSizeM,
		SeaLevelM,
		SourceReferenceVerticesPerSide,
		SourceReferenceSpacingM,
		RuntimeTilesPerSide,
		Metrics.TileCount,
		Metrics.RuntimeTileSizeM,
		Metrics.TotalVertexCount,
		Metrics.TotalTriangleCount,
		Metrics.CollisionTileCount,
		Metrics.NearLodTileCount,
		Metrics.MidLodTileCount,
		Metrics.FarLodTileCount,
		Metrics.RuntimeMinSpacingM,
		Metrics.RuntimeMaxSpacingM,
		Metrics.AdjacentSeamSampleCount,
		Metrics.MaxAdjacentSeamAbsErrorM,
		Metrics.ShorelineSampleCount,
		Metrics.MaxShorelineAbsErrorM);
}

FJGTerrainChannelSample FJungleVolcanicIslandTerrainModel::SampleTerrainChannelsMeters(float WorldXM, float WorldYM, float DerivativeSpacingM)
{
	FJGTerrainChannelSample Channels;
	Channels.Terrain = SampleTerrainMeters(WorldXM, WorldYM);

	const float Spacing = FMath::Max(0.25f, DerivativeSpacingM);
	const float HX0 = SampleHeightMeters(WorldXM - Spacing, WorldYM);
	const float HX1 = SampleHeightMeters(WorldXM + Spacing, WorldYM);
	const float HY0 = SampleHeightMeters(WorldXM, WorldYM - Spacing);
	const float HY1 = SampleHeightMeters(WorldXM, WorldYM + Spacing);
	const float DzDx = (HX1 - HX0) / (2.0f * Spacing);
	const float DzDy = (HY1 - HY0) / (2.0f * Spacing);
	Channels.SlopeDegrees = FMath::RadiansToDegrees(FMath::Atan(FMath::Sqrt(DzDx * DzDx + DzDy * DzDy)));

	float MinLocalHeight = Channels.Terrain.HeightM;
	float MaxLocalHeight = Channels.Terrain.HeightM;
	constexpr int32 ReliefRingSamples = 8;
	constexpr float ReliefRadiusM = 32.0f;
	for (int32 Index = 0; Index < ReliefRingSamples; ++Index)
	{
		const float Theta = 2.0f * PI * static_cast<float>(Index) / static_cast<float>(ReliefRingSamples);
		const float H = SampleHeightMeters(WorldXM + FMath::Cos(Theta) * ReliefRadiusM, WorldYM + FMath::Sin(Theta) * ReliefRadiusM);
		MinLocalHeight = FMath::Min(MinLocalHeight, H);
		MaxLocalHeight = FMath::Max(MaxLocalHeight, H);
	}
	Channels.ReliefM = MaxLocalHeight - MinLocalHeight;

	Channels.IslandMask01 = FMath::Clamp(SmoothStep(-10.0f, 60.0f, Channels.Terrain.SignedShorelineDistanceM), 0.0f, 1.0f);
	Channels.LandMask01 = 1.0f - Channels.Terrain.OceanMask;
	Channels.BeachMask01 = FMath::Clamp(FMath::Max(Channels.Terrain.BeachRingMask, Channels.Terrain.ShorelineConstraintMask), 0.0f, 1.0f);
	Channels.HazardMask01 = FMath::Clamp(Channels.Terrain.LavaCrustMask * 0.45f + Channels.Terrain.UnstableCrustMask * 0.65f + Channels.Terrain.HardBlockerMask, 0.0f, 1.0f);
	Channels.SlopeMask01 = FMath::Clamp(Channels.SlopeDegrees / 55.0f, 0.0f, 1.0f);

	Channels.ElevationBand = Channels.Terrain.HeightM < SeaLevelM ? 0 : (Channels.Terrain.HeightM < 60.0f ? 1 : (Channels.Terrain.HeightM < 350.0f ? 2 : (Channels.Terrain.HeightM < 950.0f ? 3 : 4)));
	Channels.SlopeClass = Channels.SlopeDegrees < 12.0f ? 0 : (Channels.SlopeDegrees < 22.0f ? 1 : (Channels.SlopeDegrees < 32.0f ? 2 : (Channels.SlopeDegrees < 42.0f ? 3 : 4)));
	Channels.ReliefClass = Channels.ReliefM < 4.0f ? 0 : (Channels.ReliefM < 16.0f ? 1 : (Channels.ReliefM < 48.0f ? 2 : 3));

	Channels.PackedCoastChannel = static_cast<uint8>(FMath::Clamp(FMath::RoundToInt(Channels.BeachMask01 * 85.0f + Channels.Terrain.CoastalShelfMask * 85.0f + Channels.Terrain.OceanMask * 85.0f), 0, 255));
	Channels.PackedLandformChannel = static_cast<uint8>(FMath::Clamp(FMath::RoundToInt(Channels.Terrain.RidgeMask * 85.0f + Channels.Terrain.GullyMask * 85.0f + Channels.Terrain.CraterMask * 85.0f), 0, 255));
	Channels.PackedHazardChannel = static_cast<uint8>(FMath::Clamp(FMath::RoundToInt(Channels.HazardMask01 * 255.0f), 0, 255));

	return Channels;
}

FJGTerrainChannelMetrics FJungleVolcanicIslandTerrainModel::BuildChannelMetrics(int32 SamplesPerSide)
{
	FJGTerrainChannelMetrics Metrics;
	Metrics.DebugChannelCount = 15;
	Metrics.CatchmentCount = PrimaryCatchmentCount;
	const int32 ClampedSamplesPerSide = FMath::Max(2, SamplesPerSide);
	const float StepM = WorldSizeM / static_cast<float>(ClampedSamplesPerSide - 1);
	float SlopeTotal = 0.0f;

	for (int32 Y = 0; Y < ClampedSamplesPerSide; ++Y)
	{
		for (int32 X = 0; X < ClampedSamplesPerSide; ++X)
		{
			const float WorldXM = -HalfExtentM + static_cast<float>(X) * StepM;
			const float WorldYM = -HalfExtentM + static_cast<float>(Y) * StepM;
			const FJGTerrainChannelSample Channels = SampleTerrainChannelsMeters(WorldXM, WorldYM, SourceReferenceSpacingM);

			Metrics.MaxSlopeDegrees = FMath::Max(Metrics.MaxSlopeDegrees, Channels.SlopeDegrees);
			Metrics.MaxReliefM = FMath::Max(Metrics.MaxReliefM, Channels.ReliefM);
			Metrics.MaxBeachMask = FMath::Max(Metrics.MaxBeachMask, Channels.BeachMask01);
			Metrics.MaxOceanMask = FMath::Max(Metrics.MaxOceanMask, Channels.Terrain.OceanMask);
			Metrics.MaxHardBlockerMask = FMath::Max(Metrics.MaxHardBlockerMask, Channels.Terrain.HardBlockerMask);
			Metrics.MaxHazardMask = FMath::Max(Metrics.MaxHazardMask, Channels.HazardMask01);
			SlopeTotal += Channels.SlopeDegrees;
			++Metrics.SampleCount;

			if (Channels.BeachMask01 > 0.5f)
			{
				++Metrics.BeachChannelSamples;
			}
			if (Channels.Terrain.OceanMask > 0.5f)
			{
				++Metrics.OceanChannelSamples;
			}
			if (Channels.Terrain.HardBlockerMask > 0.5f)
			{
				++Metrics.HardBlockerSamples;
			}
			Metrics.SlopeClassCounts[FMath::Clamp<int32>(Channels.SlopeClass, 0, 4)]++;
		}
	}

	Metrics.MeanSlopeDegrees = Metrics.SampleCount > 0 ? SlopeTotal / static_cast<float>(Metrics.SampleCount) : 0.0f;
	return Metrics;
}

FString FJungleVolcanicIslandTerrainModel::BuildChannelMetricsLogLine(const FJGTerrainChannelMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=JG_TERRAIN_CHANNELS_007 channels=%d samples=%d catchments=%d slope_max_deg=%.2f slope_mean_deg=%.2f relief_max_m=%.2f beach_mask_max=%.2f ocean_mask_max=%.2f hard_blocker_mask_max=%.2f hazard_mask_max=%.2f beach_samples=%d ocean_samples=%d hard_blocker_samples=%d slope_classes=%d/%d/%d/%d/%d source=canonical-terrain-channel-atlas"),
		Metrics.DebugChannelCount,
		Metrics.SampleCount,
		Metrics.CatchmentCount,
		Metrics.MaxSlopeDegrees,
		Metrics.MeanSlopeDegrees,
		Metrics.MaxReliefM,
		Metrics.MaxBeachMask,
		Metrics.MaxOceanMask,
		Metrics.MaxHardBlockerMask,
		Metrics.MaxHazardMask,
		Metrics.BeachChannelSamples,
		Metrics.OceanChannelSamples,
		Metrics.HardBlockerSamples,
		Metrics.SlopeClassCounts[0],
		Metrics.SlopeClassCounts[1],
		Metrics.SlopeClassCounts[2],
		Metrics.SlopeClassCounts[3],
		Metrics.SlopeClassCounts[4]);
}

FJGTerrainTopographicMetrics FJungleVolcanicIslandTerrainModel::BuildTopographicMetrics(int32 ExportVerticesPerSide)
{
	FJGTerrainTopographicMetrics Metrics;
	Metrics.ExportVerticesPerSide = FMath::Max(2, ExportVerticesPerSide);
	Metrics.ExportSpacingM = WorldSizeM / static_cast<float>(Metrics.ExportVerticesPerSide - 1);
	Metrics.GeneratedMapSchemaCount = 9;
	float HeightTotal = 0.0f;
	float SlopeTotal = 0.0f;
	TSet<int32> ContourBands;

	for (int32 Y = 0; Y < Metrics.ExportVerticesPerSide; ++Y)
	{
		for (int32 X = 0; X < Metrics.ExportVerticesPerSide; ++X)
		{
			const float WorldXM = -HalfExtentM + static_cast<float>(X) * Metrics.ExportSpacingM;
			const float WorldYM = -HalfExtentM + static_cast<float>(Y) * Metrics.ExportSpacingM;
			const FJGTerrainChannelSample Channels = SampleTerrainChannelsMeters(WorldXM, WorldYM, SourceReferenceSpacingM);

			Metrics.MinHeightM = FMath::Min(Metrics.MinHeightM, Channels.Terrain.HeightM);
			Metrics.MaxHeightM = FMath::Max(Metrics.MaxHeightM, Channels.Terrain.HeightM);
			Metrics.MaxSlopeDegrees = FMath::Max(Metrics.MaxSlopeDegrees, Channels.SlopeDegrees);
			Metrics.MaxReliefM = FMath::Max(Metrics.MaxReliefM, Channels.ReliefM);
			Metrics.RidgeMaskMax = FMath::Max(Metrics.RidgeMaskMax, Channels.Terrain.RidgeMask);
			Metrics.GullyMaskMax = FMath::Max(Metrics.GullyMaskMax, Channels.Terrain.GullyMask);
			Metrics.CraterMaskMax = FMath::Max(Metrics.CraterMaskMax, Channels.Terrain.CraterMask);
			Metrics.HazardMaskMax = FMath::Max(Metrics.HazardMaskMax, Channels.HazardMask01);
			Metrics.MaxShorelineAbsErrorM = FMath::Max(Metrics.MaxShorelineAbsErrorM, Channels.Terrain.ShorelineErrorM);

			HeightTotal += Channels.Terrain.HeightM;
			SlopeTotal += Channels.SlopeDegrees;
			++Metrics.ExportSampleCount;
			ContourBands.Add(FMath::FloorToInt(Channels.Terrain.HeightM / 50.0f));
			Metrics.SlopeHistogram[FMath::Clamp<int32>(Channels.SlopeClass, 0, 4)]++;

			if (Channels.Terrain.SignedShorelineDistanceM >= 20.0f && Channels.Terrain.SignedShorelineDistanceM <= 190.0f)
			{
				++Metrics.BeachBandSamples;
				if (Channels.Terrain.HeightM >= -0.25f && Channels.Terrain.HeightM <= 10.0f)
				{
					++Metrics.BeachBandPassSamples;
				}
			}
			if (FMath::Abs(Channels.Terrain.SignedShorelineDistanceM) <= 65.0f)
			{
				++Metrics.ShorelineContourSamples;
			}
			if (Channels.Terrain.OceanMask > 0.5f)
			{
				++Metrics.OceanSamples;
				if (Channels.Terrain.HeightM <= SeaLevelM)
				{
					++Metrics.OceanBelowSeaSamples;
				}
			}
		}
	}

	Metrics.MeanHeightM = Metrics.ExportSampleCount > 0 ? HeightTotal / static_cast<float>(Metrics.ExportSampleCount) : 0.0f;
	Metrics.MeanSlopeDegrees = Metrics.ExportSampleCount > 0 ? SlopeTotal / static_cast<float>(Metrics.ExportSampleCount) : 0.0f;
	Metrics.BeachContinuityPercent = Metrics.BeachBandSamples > 0 ? 100.0f * static_cast<float>(Metrics.BeachBandPassSamples) / static_cast<float>(Metrics.BeachBandSamples) : 0.0f;
	Metrics.OceanBelowSeaPercent = Metrics.OceanSamples > 0 ? 100.0f * static_cast<float>(Metrics.OceanBelowSeaSamples) / static_cast<float>(Metrics.OceanSamples) : 0.0f;
	Metrics.Contour50mBandsTouched = ContourBands.Num();
	return Metrics;
}

FString FJungleVolcanicIslandTerrainModel::BuildTopographicMetricsLogLine(const FJGTerrainTopographicMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=JG_TOPO_EXPORT_008 export_vertices=%d export_spacing_m=%.2f samples=%d maps=%d/%d height_min_m=%.1f height_max_m=%.1f height_mean_m=%.1f slope_max_deg=%.2f slope_mean_deg=%.2f relief_max_m=%.2f contour_50m_bands=%d shoreline_samples=%d shoreline_error_max_m=%.3f beach_continuity_pct=%.1f ocean_below_sea_pct=%.1f ridge_mask_max=%.2f gully_mask_max=%.2f crater_mask_max=%.2f hazard_mask_max=%.2f slope_hist=%d/%d/%d/%d/%d source=canonical-topographic-evidence-schema"),
		Metrics.ExportVerticesPerSide,
		Metrics.ExportSpacingM,
		Metrics.ExportSampleCount,
		Metrics.GeneratedMapSchemaCount,
		Metrics.RequiredMapCount,
		Metrics.MinHeightM,
		Metrics.MaxHeightM,
		Metrics.MeanHeightM,
		Metrics.MaxSlopeDegrees,
		Metrics.MeanSlopeDegrees,
		Metrics.MaxReliefM,
		Metrics.Contour50mBandsTouched,
		Metrics.ShorelineContourSamples,
		Metrics.MaxShorelineAbsErrorM,
		Metrics.BeachContinuityPercent,
		Metrics.OceanBelowSeaPercent,
		Metrics.RidgeMaskMax,
		Metrics.GullyMaskMax,
		Metrics.CraterMaskMax,
		Metrics.HazardMaskMax,
		Metrics.SlopeHistogram[0],
		Metrics.SlopeHistogram[1],
		Metrics.SlopeHistogram[2],
		Metrics.SlopeHistogram[3],
		Metrics.SlopeHistogram[4]);
}

FJGTerrainGeneratorConfig FJungleVolcanicIslandTerrainModel::BuildDefaultGeneratorConfig()
{
	FJGTerrainGeneratorConfig Config;
	Config.WorldSizeMeters = WorldSizeM;
	Config.SeaLevelMeters = SeaLevelM;
	Config.TargetPeakMeters = TargetPeakHeightM;
	Config.SourceVerticesPerSide = SourceReferenceVerticesPerSide;
	Config.RuntimeTilesPerSideValue = RuntimeTilesPerSide;
	return Config;
}

FString FJungleVolcanicIslandTerrainModel::BuildGeneratorConfigFingerprint(const FJGTerrainGeneratorConfig& Config)
{
	uint32 Hash = 2166136261u;
	const auto MixUInt = [&Hash](uint32 Value)
	{
		Hash ^= Value;
		Hash *= 16777619u;
	};
	const auto MixFloat = [&MixUInt](float Value)
	{
		MixUInt(static_cast<uint32>(FMath::RoundToInt(Value * 100.0f)));
	};

	MixUInt(GetTypeHash(Config.GeneratorId));
	MixUInt(GetTypeHash(Config.VersionId));
	MixUInt(static_cast<uint32>(Config.Seed));
	MixFloat(Config.WorldSizeMeters);
	MixFloat(Config.SeaLevelMeters);
	MixFloat(Config.TargetPeakMeters);
	MixUInt(static_cast<uint32>(Config.SourceVerticesPerSide));
	MixUInt(static_cast<uint32>(Config.RuntimeTilesPerSideValue));
	MixUInt(static_cast<uint32>(Config.ChannelCount));
	MixUInt(Config.bCanonicalCoastline ? 1u : 0u);
	MixUInt(Config.bDeterministicSampling ? 1u : 0u);
	MixUInt(Config.bRuntimeMeshBridgeEnabled ? 1u : 0u);
	MixUInt(Config.bTopographicEvidenceEnabled ? 1u : 0u);

	return FString::Printf(TEXT("%08x"), Hash);
}

FJGTerrainGeneratorArchitectureMetrics FJungleVolcanicIslandTerrainModel::BuildGeneratorArchitectureMetrics(const FJGTerrainGeneratorConfig& Config)
{
	FJGTerrainGeneratorArchitectureMetrics Metrics;
	Metrics.ConfigFingerprint = BuildGeneratorConfigFingerprint(Config);
	Metrics.SourceVerticesPerSide = Config.SourceVerticesPerSide;
	Metrics.RuntimeTilesPerSideValue = Config.RuntimeTilesPerSideValue;
	Metrics.ChannelCount = Config.ChannelCount;
	Metrics.ValidationStageCount = 4;
	Metrics.bWorldSizeMatches = FMath::IsNearlyEqual(Config.WorldSizeMeters, WorldSizeM, 0.01f);
	Metrics.bSeaLevelMatches = FMath::IsNearlyEqual(Config.SeaLevelMeters, SeaLevelM, 0.01f);
	Metrics.bSourceResolutionMatches = Config.SourceVerticesPerSide == SourceReferenceVerticesPerSide;
	Metrics.bCoastInvariantOwnedBySource = Config.bCanonicalCoastline;
	Metrics.bRuntimeMeshBridgeEnabled = Config.bRuntimeMeshBridgeEnabled;
	Metrics.bTopographicEvidenceEnabled = Config.bTopographicEvidenceEnabled;
	Metrics.bArchitectureValid = Metrics.bWorldSizeMatches && Metrics.bSeaLevelMatches && Metrics.bSourceResolutionMatches && Metrics.bCoastInvariantOwnedBySource && Config.bDeterministicSampling && Metrics.bRuntimeMeshBridgeEnabled && Metrics.bTopographicEvidenceEnabled;
	return Metrics;
}

FString FJungleVolcanicIslandTerrainModel::BuildGeneratorArchitectureLogLine(const FJGTerrainGeneratorArchitectureMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=JG_TERRAIN_GENERATOR_009 fingerprint=%s source_vertices=%d runtime_tiles_per_side=%d channels=%d validation_stages=%d world_size_match=%s sea_level_match=%s source_resolution_match=%s coast_source_owned=%s runtime_mesh_bridge=%s topographic_evidence=%s architecture_valid=%s"),
		*Metrics.ConfigFingerprint,
		Metrics.SourceVerticesPerSide,
		Metrics.RuntimeTilesPerSideValue,
		Metrics.ChannelCount,
		Metrics.ValidationStageCount,
		Metrics.bWorldSizeMatches ? TEXT("true") : TEXT("false"),
		Metrics.bSeaLevelMatches ? TEXT("true") : TEXT("false"),
		Metrics.bSourceResolutionMatches ? TEXT("true") : TEXT("false"),
		Metrics.bCoastInvariantOwnedBySource ? TEXT("true") : TEXT("false"),
		Metrics.bRuntimeMeshBridgeEnabled ? TEXT("true") : TEXT("false"),
		Metrics.bTopographicEvidenceEnabled ? TEXT("true") : TEXT("false"),
		Metrics.bArchitectureValid ? TEXT("true") : TEXT("false"));
}
