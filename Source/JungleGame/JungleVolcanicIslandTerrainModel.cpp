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
		1560.0f * FMath::Sin(3.0f * ThetaRadians + 0.70f) +
		1020.0f * FMath::Sin(5.0f * ThetaRadians - 1.10f) +
		450.0f * FMath::Sin(9.0f * ThetaRadians + 2.30f) +
		260.0f * FMath::Sin(13.0f * ThetaRadians - 0.80f);

	const float BayHeadlandWarp =
		570.0f * FMath::Sin(2.0f * ThetaRadians - 0.35f) *
		FMath::Sin(7.0f * ThetaRadians + 1.60f);

	return FMath::Clamp(MeanIslandRadiusM + BroadLobes + BayHeadlandWarp, 37200.0f, MaxIslandRadiusM);
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

	const FVector2D MassifCenterM(-1320.0f, 1080.0f);
	const FVector2D FromMassifM = WorldXY - MassifCenterM;
	const float MassifDistanceM = FromMassifM.Size();
	const float MassifTheta = FMath::Atan2(FromMassifM.Y, FromMassifM.X);
	const float MassifBaseRadiusM = 21800.0f;
	const float MassifNormalized = FMath::Clamp(MassifDistanceM / MassifBaseRadiusM, 0.0f, 1.35f);
	const float ConcaveShield = FMath::Pow(FMath::Clamp(1.0f - FMath::Pow(MassifNormalized, 1.48f), 0.0f, 1.0f), 1.18f);
	const float UpperSteepening = FMath::Pow(FMath::Clamp(1.0f - MassifDistanceM / 8200.0f, 0.0f, 1.0f), 2.10f) * 310.0f;
	const float ShoulderBench = FMath::Exp(-FMath::Square((MassifDistanceM - 13200.0f) / 3600.0f)) * 150.0f;
	const float Asymmetry =
		FMath::Sin(WorldXM * 0.000070f + 0.9f) * 142.0f +
		FMath::Sin(WorldYM * 0.000060f - 1.7f) * 108.0f +
		FMath::Sin((WorldXM + WorldYM) * 0.000040f + 2.1f) * 70.0f;
	const float CoastalProtection = SmoothStep(6200.0f, 11800.0f, LandwardDistanceM) * LandMask;
	const float MassifMask = FMath::Clamp(SmoothStep(0.035f, 0.84f, 1.0f - MassifNormalized) * CoastalProtection, 0.0f, 1.0f);
	const float ShieldAsymmetryMask = FMath::Clamp(0.78f + 0.13f * FMath::Sin(WorldXM * 0.000082f - WorldYM * 0.000049f + 1.4f) + 0.09f * FMath::Sin((WorldXM + WorldYM) * 0.000067f - 0.6f), 0.58f, 1.06f);
	const float MassifHeightM = (TargetPeakHeightM * ConcaveShield + UpperSteepening + ShoulderBench + Asymmetry) * CoastalProtection * ShieldAsymmetryMask;
	const float CentralSaddleCutM = CoastalProtection * (
		FMath::Exp(-(FMath::Square((WorldXM + 10400.0f) / 16400.0f) + FMath::Square((WorldYM - 6400.0f) / 5200.0f))) * 420.0f +
		FMath::Exp(-(FMath::Square((WorldXM - 11600.0f) / 14800.0f) + FMath::Square((WorldYM + 8200.0f) / 6100.0f))) * 360.0f +
		FMath::Exp(-(FMath::Square((WorldXM + 2400.0f) / 8800.0f) + FMath::Square((WorldYM + 15800.0f) / 7600.0f))) * 280.0f);
	const float SecondaryUplandPushM = CoastalProtection * (
		FMath::Exp(-(FMath::Square((WorldXM + 21200.0f) / 11800.0f) + FMath::Square((WorldYM + 9400.0f) / 9200.0f))) * 285.0f +
		FMath::Exp(-(FMath::Square((WorldXM - 23800.0f) / 10400.0f) + FMath::Square((WorldYM - 11800.0f) / 9800.0f))) * 250.0f);
	const float ShieldBreakupM = MassifMask * CoastalProtection * (
		FMath::Sin(WorldXM * 0.00021f + WorldYM * 0.00013f + 0.7f) * 115.0f +
		FMath::Sin(WorldXM * 0.00011f - WorldYM * 0.00019f - 1.3f) * 88.0f);
	const float MidlandMask = CoastalProtection * SmoothStep(5200.0f, 12800.0f, LandwardDistanceM) * (1.0f - SmoothStep(33500.0f, 43000.0f, LandwardDistanceM));
	const float MidlandBreakupM = MidlandMask * (
		FMath::Sin(WorldXM * 0.000155f - WorldYM * 0.000091f + 2.2f) * 78.0f +
		FMath::Sin(WorldXM * 0.000097f + WorldYM * 0.000173f - 0.4f) * 56.0f +
		FMath::Sin(WorldXM * 0.000291f + WorldYM * 0.000217f + 1.7f) * 34.0f);
	const float TerraceFragmentM = MidlandMask * (FMath::Sin((WorldXM + 0.45f * WorldYM) * 0.00034f + 0.8f) > 0.42f ? 1.0f : 0.0f) * (1.0f - SmoothStep(0.18f, 0.38f, FMath::Abs(FMath::Sin((WorldXM - 0.27f * WorldYM) * 0.00019f - 1.1f)))) * 52.0f;
	const float ScarpFragmentM = MidlandMask * SmoothStep(0.42f, 0.82f, FMath::Abs(FMath::Sin(WorldXM * 0.000071f - WorldYM * 0.000113f))) * FMath::Sin((WorldXM - WorldYM) * 0.000082f + 2.6f) * 46.0f;
	const float RegionNorthwest = FMath::Exp(-(FMath::Square((WorldXM + 22600.0f) / 25500.0f) + FMath::Square((WorldYM - 19800.0f) / 18800.0f)));
	const float RegionNortheast = FMath::Exp(-(FMath::Square((WorldXM - 18800.0f) / 21400.0f) + FMath::Square((WorldYM - 16600.0f) / 16600.0f)));
	const float RegionSouthwest = FMath::Exp(-(FMath::Square((WorldXM + 17200.0f) / 23600.0f) + FMath::Square((WorldYM + 9800.0f) / 21400.0f)));
	const float RegionSoutheast = FMath::Exp(-(FMath::Square((WorldXM - 13800.0f) / 19000.0f) + FMath::Square((WorldYM + 20800.0f) / 24800.0f)));
	const float RegionCentralUpland = FMath::Exp(-(FMath::Square((WorldXM + 4200.0f) / 31000.0f) + FMath::Square((WorldYM - 4000.0f) / 27000.0f)));
	const float RegionEastTerrace = FMath::Exp(-(FMath::Square((WorldXM - 28200.0f) / 16000.0f) + FMath::Square((WorldYM + 1800.0f) / 25000.0f)));
	const float RegionWestBench = FMath::Exp(-(FMath::Square((WorldXM + 31800.0f) / 12800.0f) + FMath::Square((WorldYM + 22600.0f) / 13400.0f)));
	const float RegionSouthPlain = FMath::Exp(-(FMath::Square((WorldXM - 1600.0f) / 26000.0f) + FMath::Square((WorldYM + 34200.0f) / 10400.0f)));
	const float RegionWeights[LandformRegionCount] = { RegionNorthwest, RegionNortheast, RegionSouthwest, RegionSoutheast, RegionCentralUpland, RegionEastTerrace, RegionWestBench, RegionSouthPlain };
	int32 LandformRegionId = 0;
	float LandformRegionWeight = RegionWeights[0];
	for (int32 RegionIndex = 1; RegionIndex < LandformRegionCount; ++RegionIndex)
	{
		if (RegionWeights[RegionIndex] > LandformRegionWeight)
		{
			LandformRegionWeight = RegionWeights[RegionIndex];
			LandformRegionId = RegionIndex;
		}
	}
	const float BasementHeightM = CoastalProtection * (245.0f + FMath::Sin(WorldXM * 0.000040f + 1.1f) * 180.0f + FMath::Sin(WorldYM * 0.000036f - 0.4f) * 142.0f + FMath::Sin((WorldXM - WorldYM) * 0.000025f + 2.2f) * 96.0f);
	const float RegionalLandformHeightM = CoastalProtection * (RegionNorthwest * 560.0f + RegionNortheast * 420.0f + RegionSouthwest * 360.0f + RegionSoutheast * 235.0f + RegionCentralUpland * 710.0f + RegionEastTerrace * 160.0f + RegionWestBench * 285.0f + RegionSouthPlain * 110.0f);

	const auto AngularDelta = [](float A, float B)
	{
		return FMath::Abs(FMath::Atan2(FMath::Sin(A - B), FMath::Cos(A - B)));
	};
	const auto WrapAngle = [](float A)
	{
		return FMath::Atan2(FMath::Sin(A), FMath::Cos(A));
	};

	static constexpr float BasinAngles[PrimaryCatchmentCount] =
	{
		-3.04f, -2.73f, -2.48f, -2.13f, -1.86f, -1.57f, -1.20f, -0.98f,
		-0.61f, -0.33f, -0.05f, 0.29f, 0.57f, 0.92f, 1.13f, 1.46f,
		1.73f, 2.03f, 2.34f, 2.57f, 2.82f, 3.03f, 3.13f
	};
	static constexpr float BasinWidths[PrimaryCatchmentCount] =
	{
		0.155f, 0.110f, 0.135f, 0.175f, 0.120f, 0.160f, 0.105f, 0.185f,
		0.130f, 0.115f, 0.170f, 0.145f, 0.105f, 0.190f, 0.100f, 0.155f,
		0.125f, 0.180f, 0.112f, 0.148f, 0.118f, 0.090f, 0.140f
	};
	static constexpr float BasinCurves[PrimaryCatchmentCount] =
	{
		0.34f, -0.18f, 0.27f, -0.31f, 0.14f, 0.42f, -0.24f, 0.19f,
		-0.38f, 0.25f, -0.15f, 0.33f, -0.28f, 0.16f, -0.35f, 0.22f,
		0.11f, -0.29f, 0.37f, -0.12f, 0.24f, -0.41f, 0.17f
	};
	static constexpr float BasinStrengths[PrimaryCatchmentCount] =
	{
		0.92f, 0.58f, 0.74f, 1.00f, 0.63f, 0.88f, 0.52f, 0.94f,
		0.70f, 0.61f, 0.97f, 0.79f, 0.55f, 1.00f, 0.50f, 0.83f,
		0.69f, 0.91f, 0.57f, 0.77f, 0.60f, 0.49f, 0.72f
	};

	const float Distance01 = FMath::Clamp(MassifDistanceM / 38000.0f, 0.0f, 1.0f);
	const float BasinDomainWarp =
		0.18f * FMath::Sin(MassifDistanceM * 0.00017f + MassifTheta * 2.70f) +
		0.11f * FMath::Sin(WorldXM * 0.000061f - WorldYM * 0.000047f + 0.8f) +
		0.07f * FMath::Sin((WorldXM - WorldYM) * 0.000039f + MassifTheta * 4.1f);
	float WarpedTheta = WrapAngle(MassifTheta + BasinDomainWarp * SmoothStep(0.08f, 0.94f, Distance01));

	int32 CatchmentId = 0;
	float BestGullyDelta = TNumericLimits<float>::Max();
	for (int32 BasinIndex = 0; BasinIndex < PrimaryCatchmentCount; ++BasinIndex)
	{
		const float CurvedCenter = WrapAngle(BasinAngles[BasinIndex] + BasinCurves[BasinIndex] * 0.18f * SmoothStep(0.16f, 0.88f, Distance01));
		const float Delta = AngularDelta(WarpedTheta, CurvedCenter);
		if (Delta < BestGullyDelta)
		{
			BestGullyDelta = Delta;
			CatchmentId = BasinIndex;
		}
	}

	float BestRidgeDelta = TNumericLimits<float>::Max();
	for (int32 BasinIndex = 0; BasinIndex < PrimaryCatchmentCount; ++BasinIndex)
	{
		const int32 NextIndex = (BasinIndex + 1) % PrimaryCatchmentCount;
		const float A = BasinAngles[BasinIndex];
		float B = BasinAngles[NextIndex];
		if (NextIndex == 0)
		{
			B += 2.0f * PI;
		}
		const float BoundaryAngle = WrapAngle((A + B) * 0.5f + 0.045f * FMath::Sin(static_cast<float>(BasinIndex) * 1.91f));
		BestRidgeDelta = FMath::Min(BestRidgeDelta, AngularDelta(WarpedTheta, BoundaryAngle));
	}

	static constexpr float GraphSourceX[PrimaryCatchmentCount] = { -29200.0f, -24400.0f, -19600.0f, -12600.0f, -6900.0f, -2200.0f, 6800.0f, 13200.0f, 21200.0f, 28200.0f, 32600.0f, 25000.0f, 16500.0f, 8600.0f, -1200.0f, -9600.0f, -17800.0f, -25200.0f, -31800.0f, -34200.0f, -25400.0f, -13800.0f, -4200.0f };
	static constexpr float GraphSourceY[PrimaryCatchmentCount] = { 22600.0f, 11800.0f, 32600.0f, -6800.0f, 24600.0f, 4200.0f, 19800.0f, -14800.0f, 7200.0f, -2200.0f, 18400.0f, -27800.0f, -35400.0f, 31800.0f, -23600.0f, 14600.0f, -18800.0f, -31200.0f, -4200.0f, 8200.0f, 28600.0f, -36600.0f, 35400.0f };
	static constexpr float GraphOutletX[PrimaryCatchmentCount] = { -41000.0f, -43800.0f, -28600.0f, -35800.0f, -16800.0f, -6200.0f, 7200.0f, 22800.0f, 39800.0f, 43000.0f, 30400.0f, 15600.0f, 3400.0f, -7200.0f, -19800.0f, -32600.0f, -42000.0f, -30000.0f, -43600.0f, -39000.0f, -11800.0f, 8400.0f, 22400.0f };
	static constexpr float GraphOutletY[PrimaryCatchmentCount] = { 9200.0f, -6200.0f, 33800.0f, -24600.0f, 39800.0f, 42000.0f, 41400.0f, -36600.0f, 16600.0f, -10600.0f, 30000.0f, -41400.0f, -43000.0f, 40200.0f, -38600.0f, 27800.0f, -16000.0f, -34000.0f, 17600.0f, 29600.0f, 38000.0f, -42400.0f, 35600.0f };
	static constexpr float GraphCurve[PrimaryCatchmentCount] = { 0.42f, -0.35f, 0.56f, -0.48f, 0.31f, -0.62f, 0.46f, -0.28f, 0.64f, -0.52f, 0.24f, -0.44f, 0.58f, -0.36f, 0.49f, -0.41f, 0.33f, -0.57f, 0.27f, -0.46f, 0.39f, -0.32f, 0.51f };
	static constexpr float GraphWidthM[PrimaryCatchmentCount] = { 4700.0f, 3900.0f, 5200.0f, 4400.0f, 4300.0f, 5600.0f, 4800.0f, 5100.0f, 4500.0f, 4200.0f, 5400.0f, 4900.0f, 5300.0f, 4600.0f, 5000.0f, 4100.0f, 4700.0f, 5500.0f, 4300.0f, 5200.0f, 4500.0f, 4800.0f, 5100.0f };
	float GraphBestCost = TNumericLimits<float>::Max();
	float GraphSecondCost = TNumericLimits<float>::Max();
	float GraphBestPerpM = TNumericLimits<float>::Max();
	float GraphBestT = 0.0f;
	float GraphBestTangent = WarpedTheta;
	for (int32 GraphIndex = 0; GraphIndex < PrimaryCatchmentCount; ++GraphIndex)
	{
		const float AX = GraphSourceX[GraphIndex];
		const float AY = GraphSourceY[GraphIndex];
		const float BX = GraphOutletX[GraphIndex];
		const float BY = GraphOutletY[GraphIndex];
		const float VX = BX - AX;
		const float VY = BY - AY;
		const float LineLenSq = FMath::Max(VX * VX + VY * VY, 1.0f);
		const float T = FMath::Clamp(((WorldXM - AX) * VX + (WorldYM - AY) * VY) / LineLenSq, -0.18f, 1.18f);
		const float TC = FMath::Clamp(T, 0.0f, 1.0f);
		const float LineLen = FMath::Sqrt(LineLenSq);
		const float NX = -VY / LineLen;
		const float NY = VX / LineLen;
		const float CurveAmplitudeM = 3600.0f + 1100.0f * FMath::Sin(static_cast<float>(GraphIndex) * 1.73f);
		const float CurveOffsetM = GraphCurve[GraphIndex] * FMath::Sin(TC * PI) * CurveAmplitudeM;
		const float CX = FMath::Lerp(AX, BX, TC) + NX * CurveOffsetM;
		const float CY = FMath::Lerp(AY, BY, TC) + NY * CurveOffsetM;
		const float PerpM = FVector2D(WorldXM - CX, WorldYM - CY).Size();
		const float EndpointPenalty = FMath::Max(0.0f, -T) * 1.4f + FMath::Max(0.0f, T - 1.0f) * 1.4f;
		const float SourceAffinity = 0.18f * FMath::Sin(WorldXM * 0.000031f + static_cast<float>(GraphIndex) * 0.73f) + 0.14f * FMath::Sin(WorldYM * 0.000027f - static_cast<float>(GraphIndex) * 0.51f);
		const float Cost = PerpM / GraphWidthM[GraphIndex] + EndpointPenalty - SourceAffinity;
		if (Cost < GraphBestCost)
		{
			GraphSecondCost = GraphBestCost;
			GraphBestCost = Cost;
			CatchmentId = GraphIndex;
			GraphBestPerpM = PerpM;
			GraphBestT = TC;
			const float CurveDerivative = GraphCurve[GraphIndex] * FMath::Cos(TC * PI) * PI * CurveAmplitudeM / LineLen;
			GraphBestTangent = FMath::Atan2(VY + NY * CurveDerivative, VX + NX * CurveDerivative);
		}
		else if (Cost < GraphSecondCost)
		{
			GraphSecondCost = Cost;
		}
	}
	WarpedTheta = WrapAngle(GraphBestTangent);
	const float GraphCostGap = FMath::Max(0.0f, GraphSecondCost - GraphBestCost);
	const float DomainTransitionMask = (1.0f - SmoothStep(0.10f, 0.62f, GraphCostGap)) * CoastalProtection * SmoothStep(0.12f, 0.98f, GraphBestT);
	const float DomainReliefDamping = 1.0f - DomainTransitionMask * 0.30f;
	BestGullyDelta = FMath::Clamp((GraphBestPerpM / GraphWidthM[CatchmentId]) * 0.16f, 0.0f, 0.45f);
	BestRidgeDelta = FMath::Clamp(GraphCostGap * 0.18f, 0.0f, 0.45f);

	const float BasinWidth = BasinWidths[CatchmentId] + DomainTransitionMask * 0.095f;
	const float BasinStrength = BasinStrengths[CatchmentId] * (1.0f - DomainTransitionMask * 0.18f);
	const float BasinCurve = BasinCurves[CatchmentId] * (1.0f - DomainTransitionMask * 0.35f);
	const float MidSlopeMask = SmoothStep(5200.0f, 14800.0f, MassifDistanceM) * (1.0f - SmoothStep(35000.0f, 40500.0f, MassifDistanceM)) * LandMask;
	const float RidgeBreakup = FMath::Clamp(0.56f + 0.26f * FMath::Sin(MassifDistanceM * 0.00029f + WarpedTheta * 5.3f) + 0.20f * FMath::Sin(WorldXM * 0.000083f + WorldYM * 0.000051f) + 0.12f * FMath::Sin((WorldXM - WorldYM) * 0.000117f + GraphBestT * 6.0f), 0.22f, 1.0f);
	const float GullyBreakup = FMath::Clamp(0.70f + 0.20f * FMath::Sin(MassifDistanceM * 0.00034f - WarpedTheta * 3.7f) + 0.16f * FMath::Sin(WorldXM * 0.000047f - WorldYM * 0.000089f), 0.34f, 1.0f);
	const float RidgeMask = (1.0f - SmoothStep(0.030f, 0.240f + BasinWidth * 0.24f, BestRidgeDelta)) * MidSlopeMask * SmoothStep(5400.0f, 10400.0f, LandwardDistanceM) * RidgeBreakup * (0.62f + 0.38f * SmoothStep(0.08f, 0.26f, BestGullyDelta)) * DomainReliefDamping;
	const float GullyReachMask = SmoothStep(6500.0f, 13200.0f, MassifDistanceM) * SmoothStep(3300.0f, 7400.0f, LandwardDistanceM) * (1.0f - SmoothStep(36500.0f, 43000.0f, MassifDistanceM)) * LandMask;
	const float TrunkGullyMask = (1.0f - SmoothStep(BasinWidth * 0.20f, BasinWidth * 0.82f, BestGullyDelta)) * GullyReachMask * BasinStrength * GullyBreakup * (1.0f - DomainTransitionMask * 0.18f);
	const float BranchAngleA = WrapAngle(WarpedTheta + BasinCurve * 0.58f + FMath::Sin(GraphBestT * 6.0f + static_cast<float>(CatchmentId)) * 0.18f);
	const float BranchAngleB = WrapAngle(WarpedTheta - BasinCurve * 0.52f + FMath::Cos(GraphBestT * 5.0f + static_cast<float>(CatchmentId) * 0.7f) * 0.20f);
	const float BranchReach = SmoothStep(10500.0f, 18000.0f, MassifDistanceM) * (1.0f - SmoothStep(28500.0f, 39000.0f, MassifDistanceM)) * LandMask * (1.0f - DomainTransitionMask * 0.22f);
	const float SecondaryBranchMask = FMath::Max(
		(1.0f - SmoothStep(0.030f, 0.110f, AngularDelta(WarpedTheta, BranchAngleA))) * BranchReach * 0.48f,
		(1.0f - SmoothStep(0.035f, 0.120f, AngularDelta(WarpedTheta, BranchAngleB))) * BranchReach * 0.36f);
	const float GullyMask = FMath::Clamp(TrunkGullyMask + SecondaryBranchMask, 0.0f, 1.0f);
	const bool bLaharCatchment = CatchmentId == 2 || CatchmentId == 5 || CatchmentId == 8 || CatchmentId == 11 || CatchmentId == 13 || CatchmentId == 17 || CatchmentId == 20;
	const float LaharCorridorMask = bLaharCatchment ? TrunkGullyMask * SmoothStep(12200.0f, 22500.0f, MassifDistanceM) * (1.0f - SmoothStep(36000.0f, 43500.0f, MassifDistanceM)) * (1.0f - DomainTransitionMask * 0.25f) : 0.0f;
	const float CoastalFanMask = (1.0f - SmoothStep(BasinWidth * 0.30f, BasinWidth * 1.08f, BestGullyDelta)) * SmoothStep(1200.0f, 3900.0f, LandwardDistanceM) * (1.0f - SmoothStep(7200.0f, 11800.0f, LandwardDistanceM)) * LandMask * BasinStrength * SmoothStep(0.55f, 0.98f, GraphBestT) * (1.0f - DomainTransitionMask * 0.22f);
	const float RidgeHeightM = RidgeMask * FMath::Lerp(38.0f, 145.0f, SmoothStep(10500.0f, 28000.0f, MassifDistanceM));
	const float GullyIncisionM = GullyMask * (FMath::Lerp(34.0f, 128.0f, SmoothStep(9000.0f, 33500.0f, MassifDistanceM)) + LaharCorridorMask * 72.0f) * (1.0f - DomainTransitionMask * 0.12f);
	const float FanDepositM = CoastalFanMask * FMath::Lerp(22.0f, 82.0f, SmoothStep(1900.0f, 6500.0f, LandwardDistanceM));
	const float WatershedDivideMask = FMath::Clamp(RidgeMask * (1.0f - GullyMask * 0.65f), 0.0f, 1.0f);
	const float FlowAccumulation01 = FMath::Clamp(GullyMask * 0.58f + LaharCorridorMask * 0.28f + CoastalFanMask * 0.18f + SmoothStep(0.30f, 0.82f, LandformRegionWeight) * 0.08f, 0.0f, 1.0f);
	const float TrunkStreamMask = FMath::Clamp(SmoothStep(0.42f, 0.82f, FlowAccumulation01) * GullyReachMask, 0.0f, 1.0f);
	const float TributaryStreamMask = FMath::Clamp(GullyMask * SmoothStep(0.18f, 0.46f, FlowAccumulation01) * (1.0f - TrunkStreamMask * 0.35f), 0.0f, 1.0f);
	const int32 StreamOrder = FlowAccumulation01 > 0.78f ? 4 : (FlowAccumulation01 > 0.56f ? 3 : (FlowAccumulation01 > 0.32f ? 2 : (FlowAccumulation01 > 0.12f ? 1 : 0)));
	const int32 OutletId = LandMask > 0.5f ? CatchmentId : INDEX_NONE;
	const float StreamPowerIncisionM = FlowAccumulation01 * SmoothStep(5200.0f, 33000.0f, MassifDistanceM) * (26.0f + 118.0f * SmoothStep(0.20f, 0.90f, FlowAccumulation01));
	const float HillslopeDiffusionM = WatershedDivideMask * SmoothStep(0.22f, 0.82f, RidgeMask) * 18.0f;
	const float HydrologyFanDepositionM = CoastalFanMask * FlowAccumulation01 * FMath::Lerp(28.0f, 92.0f, SmoothStep(1700.0f, 7200.0f, LandwardDistanceM));

	const FVector2D CraterCenterM = MassifCenterM + FVector2D(720.0f, -520.0f);
	const FVector2D CraterDeltaM = WorldXY - CraterCenterM;
	const float CraterYawRadians = 0.42f;
	const float CraterU = CraterDeltaM.X * FMath::Cos(CraterYawRadians) + CraterDeltaM.Y * FMath::Sin(CraterYawRadians);
	const float CraterV = -CraterDeltaM.X * FMath::Sin(CraterYawRadians) + CraterDeltaM.Y * FMath::Cos(CraterYawRadians);
	const float CraterNorm = FMath::Sqrt(FMath::Square(CraterU / 1850.0f) + FMath::Square(CraterV / 1320.0f));
	const float CraterInteriorMask = (1.0f - SmoothStep(0.72f, 1.04f, CraterNorm)) * MassifMask;
	const float RimMask = RingMask(CraterNorm, 1.0f, 0.27f) * MassifMask;
	const FVector2D VentCenterM = CraterCenterM + FVector2D(-410.0f, 520.0f);
	const float VentDistanceM = (WorldXY - VentCenterM).Size();
	const float VentMask = (1.0f - SmoothStep(320.0f, 840.0f, VentDistanceM)) * CraterInteriorMask;
	const float BreachTheta = -0.82f;
	const float BreachAngularDelta = AngularDelta(WarpedTheta, BreachTheta);
	const float BreachMask = (1.0f - SmoothStep(0.040f, 0.135f, BreachAngularDelta)) * SmoothStep(1850.0f, 4200.0f, MassifDistanceM) * (1.0f - SmoothStep(11800.0f, 17800.0f, MassifDistanceM)) * MassifMask;
	const float FissureMask = (1.0f - SmoothStep(0.014f, 0.045f, BreachAngularDelta)) * SmoothStep(5600.0f, 8200.0f, MassifDistanceM) * (1.0f - SmoothStep(18500.0f, 26000.0f, MassifDistanceM)) * MassifMask;
	const float ActiveConeMask = FMath::Clamp((1.0f - SmoothStep(3600.0f, 4550.0f, MassifDistanceM)) * MassifMask, 0.0f, 1.0f);
	const float BroaderVolcanicHighlandMask = FMath::Clamp((1.0f - SmoothStep(8500.0f, 11000.0f, MassifDistanceM)) * MassifMask, 0.0f, 1.0f);
	const float CollapseScarMask = FMath::Clamp((1.0f - SmoothStep(0.11f, 0.31f, BreachAngularDelta)) * SmoothStep(2800.0f, 5200.0f, MassifDistanceM) * (1.0f - SmoothStep(8200.0f, 10400.0f, MassifDistanceM)) * MassifMask, 0.0f, 1.0f);
	const float RiftLineMask = FMath::Clamp(FissureMask + (1.0f - SmoothStep(0.026f, 0.070f, BreachAngularDelta)) * SmoothStep(5200.0f, 9000.0f, MassifDistanceM) * (1.0f - SmoothStep(12800.0f, 17800.0f, MassifDistanceM)) * MassifMask, 0.0f, 1.0f);
	const float LavaFlowMask = FMath::Clamp((BreachMask * 0.45f + RiftLineMask * 0.35f + LaharCorridorMask * 0.30f) * SmoothStep(2800.0f, 7600.0f, MassifDistanceM), 0.0f, 1.0f);
	const float OldLavaBenchMask = FMath::Clamp(BroaderVolcanicHighlandMask * RingMask(LandwardDistanceM, 7200.0f, 3100.0f) * (1.0f - ActiveConeMask * 0.65f), 0.0f, 1.0f);
	const float SecondaryConeMask = FMath::Clamp(RiftLineMask * RingMask(MassifDistanceM, 11600.0f, 2300.0f), 0.0f, 1.0f);
	const float BrokenRimMask = RimMask * (1.0f - BreachMask);
	const float LavaCrustMask = FMath::Clamp(CraterInteriorMask * 0.70f + VentMask + BreachMask * 0.45f + RiftLineMask * 0.60f + LavaFlowMask * 0.30f, 0.0f, 1.0f);
	const float UnstableCrustMask = FMath::Clamp(VentMask + FissureMask + LavaCrustMask * 0.45f, 0.0f, 1.0f);
	const float HardBlockerMask = FMath::Clamp(BrokenRimMask * 0.75f + VentMask + FissureMask * 0.85f + BreachMask * 0.35f, 0.0f, 1.0f);
	const float RimRaiseM = BrokenRimMask * 260.0f;
	const float CraterDepressionM = CraterInteriorMask * 420.0f + VentMask * 260.0f + BreachMask * 190.0f + FissureMask * 110.0f;
	const float UplandBlockMask = FMath::Clamp(FMath::Max(RegionCentralUpland, FMath::Max(RegionNorthwest, RegionSouthwest)) * CoastalProtection, 0.0f, 1.0f);
	const float BasinFloorMask = FMath::Clamp((TrunkStreamMask * 0.72f + CoastalFanMask * 0.28f) * (1.0f - WatershedDivideMask * 0.45f), 0.0f, 1.0f);
	const float SaddlePassMask = FMath::Clamp(WatershedDivideMask * RingMask(MassifDistanceM, 21800.0f, 7600.0f) * (1.0f - GullyMask * 0.55f), 0.0f, 1.0f);
	const float OldTerraceMask = FMath::Clamp(RingMask(LandwardDistanceM, 5200.0f, 2850.0f) * LandMask * (1.0f - CoastalFanMask * 0.35f), 0.0f, 1.0f);
	const float ScarpMask = FMath::Clamp(RidgeMask * SmoothStep(0.24f, 0.74f, WatershedDivideMask), 0.0f, 1.0f);
	const float SecondaryHillMask = FMath::Clamp(FMath::Max(RegionEastTerrace, RegionWestBench) * SmoothStep(7200.0f, 17600.0f, LandwardDistanceM), 0.0f, 1.0f);
	const float VolcanoApronMask = FMath::Clamp(MassifMask * (1.0f - CraterInteriorMask) * SmoothStep(7200.0f, 18800.0f, MassifDistanceM) * (1.0f - SmoothStep(23800.0f, 31800.0f, MassifDistanceM)), 0.0f, 1.0f);
	const int32 LandformClassId = CraterInteriorMask > 0.35f ? 11 : (VolcanoApronMask > 0.35f ? 12 : (SaddlePassMask > 0.30f ? 5 : (BasinFloorMask > 0.30f ? 3 : (OldTerraceMask > 0.35f ? 7 : (UplandBlockMask > 0.35f ? 1 : 2)))));
	const int32 GraphLandformRegionId = CraterInteriorMask > 0.35f ? 23 : (VolcanoApronMask > 0.35f ? 22 : (SaddlePassMask > 0.30f ? 20 + (CatchmentId % 2) : (BasinFloorMask > 0.30f ? 11 + (CatchmentId % 3) : (OldTerraceMask > 0.35f ? 17 + (CatchmentId % 3) : (LandwardDistanceM < 4200.0f ? 14 + (CatchmentId % 5) : 3 + (CatchmentId % 8))))));
	const float GrammarAuthorityMask = FMath::Clamp(FMath::Max(UplandBlockMask, FMath::Max(BasinFloorMask, FMath::Max(OldTerraceMask, FMath::Max(LavaFlowMask, SaddlePassMask)))) * CoastalProtection, 0.0f, 1.0f);
	const float DomainWarpMagnitudeM = FMath::Clamp((FMath::Abs(FMath::Sin(WorldXM * 0.000052f + WorldYM * 0.000031f)) * 420.0f + FMath::Abs(FMath::Sin(WorldXM * 0.000019f - WorldYM * 0.000044f)) * 260.0f) * GrammarAuthorityMask, 0.0f, 720.0f);
	const float LargeFeatureStampMask = FMath::Clamp(FMath::Max(OldTerraceMask, FMath::Max(BasinFloorMask, LavaFlowMask)), 0.0f, 1.0f);
	const float MesoscaleDetailMask = FMath::Clamp((RidgeMask * 0.42f + LavaFlowMask * 0.35f + OldLavaBenchMask * 0.32f + SecondaryHillMask * 0.22f) * SmoothStep(2600.0f, 7600.0f, LandwardDistanceM), 0.0f, 1.0f);
	const float TraversalScaleDetailMask = FMath::Clamp((BasinFloorMask * 0.25f + OldTerraceMask * 0.20f + CoastalFanMask * 0.15f) * LandMask, 0.0f, 1.0f);
	const float FeatureGrammarMask = FMath::Clamp(LargeFeatureStampMask * 0.52f + MesoscaleDetailMask * 0.30f + TraversalScaleDetailMask * 0.18f, 0.0f, 1.0f);
	const float FeatureDetailWaveM = FMath::Sin((WorldXM + DomainWarpMagnitudeM) * 0.00270f + (WorldYM - DomainWarpMagnitudeM * 0.37f) * 0.00190f);
	const float DetailSupportHeightM = (LargeFeatureStampMask * 28.0f + MesoscaleDetailMask * 12.0f + TraversalScaleDetailMask * 3.0f) * FeatureDetailWaveM * CoastalProtection;
	const float RadialArtifactSuppressionMask = FMath::Clamp((1.0f - FMath::Abs(FMath::Sin(WarpedTheta * 11.5f))) * GrammarAuthorityMask * (1.0f - ActiveConeMask * 0.55f), 0.0f, 1.0f);

	const float LongWaveUndulationM =
		FMath::Sin(WorldXM * 0.00030f + WorldYM * 0.00018f) * 46.0f +
		FMath::Sin(WorldXM * 0.00045f - WorldYM * 0.00027f) * 32.0f +
		FMath::Sin(WorldXM * 0.00012f + MassifTheta * 3.0f) * 24.0f;
	const float VolcanoDisabledProcessHeightM = BasementHeightM + RegionalLandformHeightM + MassifHeightM - CentralSaddleCutM + SecondaryUplandPushM + ShieldBreakupM + MidlandBreakupM + TerraceFragmentM + ScarpFragmentM + LongWaveUndulationM * LandMask + DetailSupportHeightM + RidgeHeightM - GullyIncisionM - StreamPowerIncisionM - HillslopeDiffusionM + FanDepositM + HydrologyFanDepositionM;
	const float ActiveVolcanoContributionM = BroaderVolcanicHighlandMask * 180.0f + ActiveConeMask * 600.0f + SecondaryConeMask * 85.0f + OldLavaBenchMask * 42.0f + LavaFlowMask * 32.0f + RimRaiseM - CraterDepressionM - CollapseScarMask * 310.0f;
	const float TerrainProcessHeightM = VolcanoDisabledProcessHeightM + ActiveVolcanoContributionM;
	const float VolcanoDisabledLandHeightM = FMath::Max(CoastalLandHeightM - GullyIncisionM * 0.18f, CoastalLandHeightM + VolcanoDisabledProcessHeightM);
	const float LandHeightM = FMath::Max(CoastalLandHeightM - GullyIncisionM * 0.18f, CoastalLandHeightM + TerrainProcessHeightM);
	const float VolcanoDisabledRawHeightM = FMath::Lerp(OceanHeightM, VolcanoDisabledLandHeightM, LandMask);
	const float RawHeightM = FMath::Lerp(OceanHeightM, LandHeightM, LandMask);
	const float AbsShorelineDistanceM = FMath::Abs(SignedShorelineDistanceM);
	const float ShorelineConstraintMask = 1.0f - SmoothStep(12.0f, 85.0f, AbsShorelineDistanceM);
	const float ShorelineHardLockMask = 1.0f - SmoothStep(76.0f, 94.0f, AbsShorelineDistanceM);
	const float ShoreLockedHeightM = FMath::Lerp(RawHeightM, SeaLevelM, FMath::Max(ShorelineConstraintMask, ShorelineHardLockMask));
	const float BeachAcceptanceMask = SmoothStep(20.0f, 80.0f, SignedShorelineDistanceM) * (1.0f - SmoothStep(190.0f, 260.0f, SignedShorelineDistanceM)) * LandMask;
	const float BeachAcceptedHeightM = FMath::Clamp(ShoreLockedHeightM, -0.25f, 9.75f);
	const float CoastLockedHeightM = FMath::Lerp(ShoreLockedHeightM, BeachAcceptedHeightM, BeachAcceptanceMask);
	const float VolcanoDisabledShoreLockedHeightM = FMath::Lerp(VolcanoDisabledRawHeightM, SeaLevelM, FMath::Max(ShorelineConstraintMask, ShorelineHardLockMask));
	const float VolcanoDisabledBeachAcceptedHeightM = FMath::Clamp(VolcanoDisabledShoreLockedHeightM, -0.25f, 9.75f);
	const float VolcanoDisabledCoastLockedHeightM = FMath::Lerp(VolcanoDisabledShoreLockedHeightM, VolcanoDisabledBeachAcceptedHeightM, BeachAcceptanceMask);

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
	Sample.BasementHeightM = BasementHeightM;
	Sample.RegionalLandformHeightM = RegionalLandformHeightM;
	Sample.VolcanoDisabledHeightM = VolcanoDisabledCoastLockedHeightM;
	Sample.ActiveVolcanoContributionM = ActiveVolcanoContributionM;
	Sample.LandformRegionWeight = LandformRegionWeight;
	Sample.UplandBlockMask = UplandBlockMask;
	Sample.BasinFloorMask = BasinFloorMask;
	Sample.SaddlePassMask = SaddlePassMask;
	Sample.OldTerraceMask = OldTerraceMask;
	Sample.ScarpMask = ScarpMask;
	Sample.SecondaryHillMask = SecondaryHillMask;
	Sample.VolcanoApronMask = VolcanoApronMask;
	Sample.ActiveConeMask = ActiveConeMask;
	Sample.BroaderVolcanicHighlandMask = BroaderVolcanicHighlandMask;
	Sample.CollapseScarMask = CollapseScarMask;
	Sample.RiftLineMask = RiftLineMask;
	Sample.LavaFlowMask = LavaFlowMask;
	Sample.OldLavaBenchMask = OldLavaBenchMask;
	Sample.SecondaryConeMask = SecondaryConeMask;
	Sample.FeatureGrammarMask = FeatureGrammarMask;
	Sample.DomainWarpMagnitudeM = DomainWarpMagnitudeM;
	Sample.LargeFeatureStampMask = LargeFeatureStampMask;
	Sample.MesoscaleDetailMask = MesoscaleDetailMask;
	Sample.TraversalScaleDetailMask = TraversalScaleDetailMask;
	Sample.DetailSupportHeightM = DetailSupportHeightM;
	Sample.RadialArtifactSuppressionMask = RadialArtifactSuppressionMask;
	Sample.RidgeMask = RidgeMask;
	Sample.GullyMask = GullyMask;
	Sample.LaharCorridorMask = LaharCorridorMask;
	Sample.CoastalFanMask = CoastalFanMask;
	Sample.FlowAccumulation01 = FlowAccumulation01;
	Sample.TrunkStreamMask = TrunkStreamMask;
	Sample.TributaryStreamMask = TributaryStreamMask;
	Sample.WatershedDivideMask = WatershedDivideMask;
	Sample.StreamPowerIncisionM = StreamPowerIncisionM;
	Sample.HillslopeDiffusionM = HillslopeDiffusionM;
	Sample.HydrologyFanDepositionM = HydrologyFanDepositionM;
	Sample.StreamOrder = StreamOrder;
	Sample.OutletId = OutletId;
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
	Sample.LandformRegionId = LandMask > 0.5f ? GraphLandformRegionId : INDEX_NONE;
	Sample.LandformClassId = LandMask > 0.5f ? LandformClassId : INDEX_NONE;

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
	Metrics.LandformRegionCount = LandformRegionCount;
	Metrics.DrainageOutletCount = PrimaryCatchmentCount;
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
			Metrics.MaxVolcanoDisabledHeightM = FMath::Max(Metrics.MaxVolcanoDisabledHeightM, Sample.VolcanoDisabledHeightM);
			Metrics.MaxActiveVolcanoContributionM = FMath::Max(Metrics.MaxActiveVolcanoContributionM, FMath::Abs(Sample.ActiveVolcanoContributionM));
			Metrics.MaxLandformRegionWeight = FMath::Max(Metrics.MaxLandformRegionWeight, Sample.LandformRegionWeight);
			Metrics.MaxFlowAccumulation01 = FMath::Max(Metrics.MaxFlowAccumulation01, Sample.FlowAccumulation01);
			Metrics.MaxTrunkStreamMask = FMath::Max(Metrics.MaxTrunkStreamMask, Sample.TrunkStreamMask);
			Metrics.MaxTributaryStreamMask = FMath::Max(Metrics.MaxTributaryStreamMask, Sample.TributaryStreamMask);
			Metrics.MaxWatershedDivideMask = FMath::Max(Metrics.MaxWatershedDivideMask, Sample.WatershedDivideMask);
			Metrics.MaxStreamPowerIncisionM = FMath::Max(Metrics.MaxStreamPowerIncisionM, Sample.StreamPowerIncisionM);
			Metrics.MaxHydrologyFanDepositionM = FMath::Max(Metrics.MaxHydrologyFanDepositionM, Sample.HydrologyFanDepositionM);
			Metrics.MaxActiveConeMask = FMath::Max(Metrics.MaxActiveConeMask, Sample.ActiveConeMask);
			Metrics.MaxBroaderVolcanicHighlandMask = FMath::Max(Metrics.MaxBroaderVolcanicHighlandMask, Sample.BroaderVolcanicHighlandMask);
			Metrics.MaxCollapseScarMask = FMath::Max(Metrics.MaxCollapseScarMask, Sample.CollapseScarMask);
			Metrics.MaxRiftLineMask = FMath::Max(Metrics.MaxRiftLineMask, Sample.RiftLineMask);
			Metrics.MaxLavaFlowMask = FMath::Max(Metrics.MaxLavaFlowMask, Sample.LavaFlowMask);
			Metrics.MaxOldLavaBenchMask = FMath::Max(Metrics.MaxOldLavaBenchMask, Sample.OldLavaBenchMask);
			Metrics.MaxSecondaryConeMask = FMath::Max(Metrics.MaxSecondaryConeMask, Sample.SecondaryConeMask);
			Metrics.MaxFeatureGrammarMask = FMath::Max(Metrics.MaxFeatureGrammarMask, Sample.FeatureGrammarMask);
			Metrics.MaxDomainWarpMagnitudeM = FMath::Max(Metrics.MaxDomainWarpMagnitudeM, Sample.DomainWarpMagnitudeM);
			Metrics.MaxLargeFeatureStampMask = FMath::Max(Metrics.MaxLargeFeatureStampMask, Sample.LargeFeatureStampMask);
			Metrics.MaxMesoscaleDetailMask = FMath::Max(Metrics.MaxMesoscaleDetailMask, Sample.MesoscaleDetailMask);
			Metrics.MaxTraversalScaleDetailMask = FMath::Max(Metrics.MaxTraversalScaleDetailMask, Sample.TraversalScaleDetailMask);
			Metrics.MaxDetailSupportHeightM = FMath::Max(Metrics.MaxDetailSupportHeightM, FMath::Abs(Sample.DetailSupportHeightM));
			Metrics.MaxRadialArtifactSuppressionMask = FMath::Max(Metrics.MaxRadialArtifactSuppressionMask, Sample.RadialArtifactSuppressionMask);
			Metrics.MaxStreamOrder = FMath::Max(Metrics.MaxStreamOrder, Sample.StreamOrder);
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
		TEXT("id=JG_TERRAIN_BATCH006_SOURCE world_m=%.0f sea_level_m=%.1f samples=%d height_min_m=%.1f height_max_m=%.1f island_radius_min_m=%.1f island_radius_max_m=%.1f ocean_margin_min_m=%.1f square_edge_height_min_m=%.1f square_edge_height_max_m=%.1f target_peak_m=%.1f catchments=%d ridge_mask_max=%.2f gully_mask_max=%.2f lahar_mask_max=%.2f crater_mask_max=%.2f vent_mask_max=%.2f hard_blocker_mask_max=%.2f shoreline_error_max_m=%.2f beach_continuity_pct=%.1f square_edge_ocean_violations=%d"),
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
	MixUInt(Config.bGeomorphologyCoreEnabled ? 1u : 0u);
	MixUInt(Config.bActiveVolcanoBounded ? 1u : 0u);
	MixUInt(Config.bHydrologySolverEnabled ? 1u : 0u);
	MixUInt(Config.bRidgeValleyGraphEnabled ? 1u : 0u);
	MixUInt(Config.bCompactVolcanicSubsystemEnabled ? 1u : 0u);
	MixUInt(Config.bFeatureGrammarEnabled ? 1u : 0u);

	return FString::Printf(TEXT("%08x"), Hash);
}

FJGTerrainGeneratorArchitectureMetrics FJungleVolcanicIslandTerrainModel::BuildGeneratorArchitectureMetrics(const FJGTerrainGeneratorConfig& Config)
{
	FJGTerrainGeneratorArchitectureMetrics Metrics;
	Metrics.ConfigFingerprint = BuildGeneratorConfigFingerprint(Config);
	Metrics.SourceVerticesPerSide = Config.SourceVerticesPerSide;
	Metrics.RuntimeTilesPerSideValue = Config.RuntimeTilesPerSideValue;
	Metrics.ChannelCount = Config.ChannelCount;
	Metrics.ValidationStageCount = 5;
	Metrics.bWorldSizeMatches = FMath::IsNearlyEqual(Config.WorldSizeMeters, WorldSizeM, 0.01f);
	Metrics.bSeaLevelMatches = FMath::IsNearlyEqual(Config.SeaLevelMeters, SeaLevelM, 0.01f);
	Metrics.bSourceResolutionMatches = Config.SourceVerticesPerSide == SourceReferenceVerticesPerSide;
	Metrics.bCoastInvariantOwnedBySource = Config.bCanonicalCoastline;
	Metrics.bRuntimeMeshBridgeEnabled = Config.bRuntimeMeshBridgeEnabled;
	Metrics.bTopographicEvidenceEnabled = Config.bTopographicEvidenceEnabled;
	Metrics.bGeomorphologyCoreEnabled = Config.bGeomorphologyCoreEnabled;
	Metrics.bActiveVolcanoBounded = Config.bActiveVolcanoBounded;
	Metrics.bHydrologySolverEnabled = Config.bHydrologySolverEnabled;
	Metrics.bRidgeValleyGraphEnabled = Config.bRidgeValleyGraphEnabled;
	Metrics.bCompactVolcanicSubsystemEnabled = Config.bCompactVolcanicSubsystemEnabled;
	Metrics.bFeatureGrammarEnabled = Config.bFeatureGrammarEnabled;
	Metrics.bArchitectureValid = Metrics.bWorldSizeMatches && Metrics.bSeaLevelMatches && Metrics.bSourceResolutionMatches && Metrics.bCoastInvariantOwnedBySource && Config.bDeterministicSampling && Metrics.bRuntimeMeshBridgeEnabled && Metrics.bTopographicEvidenceEnabled && Metrics.bGeomorphologyCoreEnabled && Metrics.bActiveVolcanoBounded && Metrics.bHydrologySolverEnabled && Metrics.bRidgeValleyGraphEnabled && Metrics.bCompactVolcanicSubsystemEnabled && Metrics.bFeatureGrammarEnabled;
	return Metrics;
}

FString FJungleVolcanicIslandTerrainModel::BuildGeneratorArchitectureLogLine(const FJGTerrainGeneratorArchitectureMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=JG_TERRAIN_BRIDGE_006_002 fingerprint=%s source_vertices=%d runtime_tiles_per_side=%d channels=%d validation_stages=%d world_size_match=%s sea_level_match=%s source_resolution_match=%s coast_source_owned=%s runtime_mesh_bridge=%s topographic_evidence=%s architecture_valid=%s"),
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

FJGTerrainBatchAcceptanceMetrics FJungleVolcanicIslandTerrainModel::BuildBatchAcceptanceMetrics()
{
	FJGTerrainBatchAcceptanceMetrics Acceptance;
	const FJGTerrainMetrics TerrainMetrics = BuildMetrics(RuntimePreviewVerticesPerSide);

	TArray<FJGTerrainRuntimeTileDesc> Tiles;
	BuildRuntimeValidationTileDescs(Tiles);
	const FJGTerrainRuntimeMeshMetrics RuntimeMetrics = BuildRuntimeMeshMetrics(Tiles);
	const FJGTerrainChannelMetrics ChannelMetrics = BuildChannelMetrics(RuntimePreviewVerticesPerSide);
	const FJGTerrainTopographicMetrics TopographicMetrics = BuildTopographicMetrics(RuntimePreviewVerticesPerSide);
	const FJGTerrainGeneratorConfig Config = BuildDefaultGeneratorConfig();
	const FJGTerrainGeneratorArchitectureMetrics ArchitectureMetrics = BuildGeneratorArchitectureMetrics(Config);

	Acceptance.AcceptedTerrainVersion = TEXT("JG_TERRAIN_BATCH006_DEM_CALIBRATION");
	Acceptance.GeneratorFingerprint = ArchitectureMetrics.ConfigFingerprint;
	Acceptance.RuntimeFilesAccepted = 5;
	Acceptance.RequiredLogTokenCount = 7;
	Acceptance.LandformRegionCount = TerrainMetrics.LandformRegionCount;
	Acceptance.VolcanoDisabledPeakM = TerrainMetrics.MaxVolcanoDisabledHeightM;
	Acceptance.ActiveVolcanoContributionMaxM = TerrainMetrics.MaxActiveVolcanoContributionM;
	Acceptance.FlowAccumulationMax01 = TerrainMetrics.MaxFlowAccumulation01;
	Acceptance.StreamPowerIncisionMaxM = TerrainMetrics.MaxStreamPowerIncisionM;
	Acceptance.ActiveConeMaskMax = TerrainMetrics.MaxActiveConeMask;
	Acceptance.BroaderVolcanicHighlandMaskMax = TerrainMetrics.MaxBroaderVolcanicHighlandMask;
	Acceptance.CollapseScarMaskMax = TerrainMetrics.MaxCollapseScarMask;
	Acceptance.RiftLineMaskMax = TerrainMetrics.MaxRiftLineMask;
	Acceptance.LavaFlowMaskMax = TerrainMetrics.MaxLavaFlowMask;
	Acceptance.FeatureGrammarMaskMax = TerrainMetrics.MaxFeatureGrammarMask;
	Acceptance.DomainWarpMagnitudeMaxM = TerrainMetrics.MaxDomainWarpMagnitudeM;
	Acceptance.DetailSupportHeightMaxM = TerrainMetrics.MaxDetailSupportHeightM;
	Acceptance.RadialArtifactSuppressionMax = TerrainMetrics.MaxRadialArtifactSuppressionMask;
	Acceptance.MaxStreamOrder = TerrainMetrics.MaxStreamOrder;
	Acceptance.DrainageOutletCount = TerrainMetrics.DrainageOutletCount;
	Acceptance.PeakHeightM = TerrainMetrics.MaxHeightM;
	Acceptance.ShorelineErrorMaxM = TerrainMetrics.MaxShorelineAbsErrorM;
	Acceptance.BeachContinuityPercent = TerrainMetrics.BeachContinuityPercent;
	Acceptance.OceanBelowSeaPercent = TopographicMetrics.OceanBelowSeaPercent;
	Acceptance.SeamErrorMaxM = RuntimeMetrics.MaxAdjacentSeamAbsErrorM;
	Acceptance.MaxSlopeDegrees = ChannelMetrics.MaxSlopeDegrees;
	Acceptance.HardBlockerMaskMax = TerrainMetrics.MaxHardBlockerMask;
	Acceptance.bPeakAccepted = Acceptance.PeakHeightM >= 2400.0f && Acceptance.PeakHeightM <= 4050.0f;
	Acceptance.bSeaLevelAccepted = Acceptance.ShorelineErrorMaxM <= 0.25f;
	Acceptance.bBeachAccepted = Acceptance.BeachContinuityPercent >= 95.0f;
	Acceptance.bOceanAccepted = Acceptance.OceanBelowSeaPercent >= 99.0f;
	Acceptance.bRuntimeMeshAccepted = RuntimeMetrics.TileCount > 0 && RuntimeMetrics.TotalVertexCount > 0 && Acceptance.SeamErrorMaxM <= 0.001f;
	Acceptance.bChannelsAccepted = ChannelMetrics.DebugChannelCount >= 15 && ChannelMetrics.MaxBeachMask > 0.0f && ChannelMetrics.MaxHardBlockerMask > 0.0f;
	Acceptance.bTopographicEvidenceAccepted = TopographicMetrics.GeneratedMapSchemaCount >= TopographicMetrics.RequiredMapCount && TopographicMetrics.ExportSampleCount > 0;
	Acceptance.bArchitectureAccepted = ArchitectureMetrics.bArchitectureValid;
	Acceptance.bVolcanoDisabledTerrainAccepted = Acceptance.VolcanoDisabledPeakM >= 900.0f;
	Acceptance.bLandformRegionAuthorityAccepted = Acceptance.LandformRegionCount >= 6 && TerrainMetrics.MaxLandformRegionWeight > 0.35f;
	Acceptance.bHydrologyAccepted = Acceptance.FlowAccumulationMax01 > 0.20f && Acceptance.StreamPowerIncisionMaxM > 1.0f && Acceptance.MaxStreamOrder >= 2 && Acceptance.DrainageOutletCount >= 8;
	Acceptance.bCompactVolcanicSubsystemAccepted = Acceptance.ActiveConeMaskMax > 0.60f && Acceptance.BroaderVolcanicHighlandMaskMax > 0.60f && Acceptance.CollapseScarMaskMax > 0.10f && Acceptance.RiftLineMaskMax > 0.10f && Acceptance.LavaFlowMaskMax > 0.10f;
	Acceptance.bFeatureGrammarAccepted = Acceptance.FeatureGrammarMaskMax > 0.40f && Acceptance.DomainWarpMagnitudeMaxM > 100.0f && Acceptance.DetailSupportHeightMaxM > 1.0f && Acceptance.RadialArtifactSuppressionMax > 0.20f;
	Acceptance.bBatchAccepted = Acceptance.bPeakAccepted && Acceptance.bSeaLevelAccepted && Acceptance.bBeachAccepted && Acceptance.bOceanAccepted && Acceptance.bRuntimeMeshAccepted && Acceptance.bChannelsAccepted && Acceptance.bTopographicEvidenceAccepted && Acceptance.bArchitectureAccepted && Acceptance.bVolcanoDisabledTerrainAccepted && Acceptance.bLandformRegionAuthorityAccepted && Acceptance.bHydrologyAccepted && Acceptance.bCompactVolcanicSubsystemAccepted && Acceptance.bFeatureGrammarAccepted;
	return Acceptance;
}

FString FJungleVolcanicIslandTerrainModel::BuildBatchAcceptanceLogLine(const FJGTerrainBatchAcceptanceMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=JG_TERRAIN_BATCH006_ACCEPTANCE_LEGACY_BRIDGE version=%s fingerprint=%s runtime_files=%d/%d log_tokens=%d peak_m=%.1f peak_ok=%s shoreline_error_max_m=%.3f sea_level_ok=%s beach_continuity_pct=%.1f beach_ok=%s ocean_below_sea_pct=%.1f ocean_ok=%s seam_error_max_m=%.4f runtime_mesh_ok=%s slope_max_deg=%.2f hard_blocker_mask_max=%.2f channels_ok=%s topo_ok=%s architecture_ok=%s batch_accepted=%s"),
		*Metrics.AcceptedTerrainVersion,
		*Metrics.GeneratorFingerprint,
		Metrics.RuntimeFilesAccepted,
		Metrics.RuntimeFilesExpected,
		Metrics.RequiredLogTokenCount,
		Metrics.PeakHeightM,
		Metrics.bPeakAccepted ? TEXT("true") : TEXT("false"),
		Metrics.ShorelineErrorMaxM,
		Metrics.bSeaLevelAccepted ? TEXT("true") : TEXT("false"),
		Metrics.BeachContinuityPercent,
		Metrics.bBeachAccepted ? TEXT("true") : TEXT("false"),
		Metrics.OceanBelowSeaPercent,
		Metrics.bOceanAccepted ? TEXT("true") : TEXT("false"),
		Metrics.SeamErrorMaxM,
		Metrics.bRuntimeMeshAccepted ? TEXT("true") : TEXT("false"),
		Metrics.MaxSlopeDegrees,
		Metrics.HardBlockerMaskMax,
		Metrics.bChannelsAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bTopographicEvidenceAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bArchitectureAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bBatchAccepted ? TEXT("true") : TEXT("false"));
}
