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
	float ShorelineConstraintMask = 0.0f;
	float ShorelineErrorM = 0.0f;
	float BeachWidthM = 0.0f;
	float MassifMask = 0.0f;
	float MassifHeightM = 0.0f;
	float BasementHeightM = 0.0f;
	float RegionalLandformHeightM = 0.0f;
	float VolcanoDisabledHeightM = 0.0f;
	float ActiveVolcanoContributionM = 0.0f;
	float LandformRegionWeight = 0.0f;
	float RidgeMask = 0.0f;
	float GullyMask = 0.0f;
	float LaharCorridorMask = 0.0f;
	float CoastalFanMask = 0.0f;
	float FlowAccumulation01 = 0.0f;
	float TrunkStreamMask = 0.0f;
	float TributaryStreamMask = 0.0f;
	float WatershedDivideMask = 0.0f;
	float StreamPowerIncisionM = 0.0f;
	float HillslopeDiffusionM = 0.0f;
	float HydrologyFanDepositionM = 0.0f;
	int32 StreamOrder = 0;
	int32 OutletId = INDEX_NONE;
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
	int32 LandformRegionId = INDEX_NONE;
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
	float MaxShorelineAbsErrorM = 0.0f;
	float BeachContinuityPercent = 0.0f;
	float MaxRidgeMask = 0.0f;
	float MaxGullyMask = 0.0f;
	float MaxLaharCorridorMask = 0.0f;
	float MaxCraterMask = 0.0f;
	float MaxVentMask = 0.0f;
	float MaxHardBlockerMask = 0.0f;
	float MaxVolcanoDisabledHeightM = TNumericLimits<float>::Lowest();
	float MaxActiveVolcanoContributionM = 0.0f;
	float MaxLandformRegionWeight = 0.0f;
	float MaxFlowAccumulation01 = 0.0f;
	float MaxTrunkStreamMask = 0.0f;
	float MaxTributaryStreamMask = 0.0f;
	float MaxWatershedDivideMask = 0.0f;
	float MaxStreamPowerIncisionM = 0.0f;
	float MaxHydrologyFanDepositionM = 0.0f;
	int32 MaxStreamOrder = 0;
	int32 SampleCount = 0;
	int32 CatchmentCount = 14;
	int32 LandformRegionCount = 0;
	int32 DrainageOutletCount = 0;
	int32 OceanEdgeSampleCount = 0;
	int32 ShorelineSampleCount = 0;
	int32 BeachSampleCount = 0;
	int32 BeachContinuitySampleCount = 0;
	int32 SquareEdgeOceanViolationCount = 0;
};

struct FJGTerrainRuntimeTileDesc
{
	int32 TileX = 0;
	int32 TileY = 0;
	int32 LodIndex = 0;
	int32 VerticesPerSide = 0;
	float MinXM = 0.0f;
	float MinYM = 0.0f;
	float MaxXM = 0.0f;
	float MaxYM = 0.0f;
	float SpacingM = 0.0f;
	bool bCollisionEnabled = false;
	FName TileName;
};

struct FJGTerrainRuntimeMeshMetrics
{
	int32 TileCount = 0;
	int32 CollisionTileCount = 0;
	int32 TotalVertexCount = 0;
	int32 TotalTriangleCount = 0;
	int32 NearLodTileCount = 0;
	int32 MidLodTileCount = 0;
	int32 FarLodTileCount = 0;
	float RuntimeTileSizeM = 0.0f;
	float SourceReferenceSpacingM = 0.0f;
	float RuntimeMinSpacingM = TNumericLimits<float>::Max();
	float RuntimeMaxSpacingM = 0.0f;
	float MaxAdjacentSeamAbsErrorM = 0.0f;
	float MaxShorelineAbsErrorM = 0.0f;
	int32 AdjacentSeamSampleCount = 0;
	int32 ShorelineSampleCount = 0;
};

enum class EJGTerrainDebugChannel : uint8
{
	Elevation,
	Island,
	Ocean,
	Beach,
	CoastalShelf,
	Massif,
	Ridge,
	Gully,
	Catchment,
	Crater,
	LavaCrust,
	UnstableCrust,
	HardBlocker,
	Slope,
	Relief
};

struct FJGTerrainChannelSample
{
	FJGTerrainSample Terrain;
	float SlopeDegrees = 0.0f;
	float ReliefM = 0.0f;
	float IslandMask01 = 0.0f;
	float LandMask01 = 0.0f;
	float BeachMask01 = 0.0f;
	float HazardMask01 = 0.0f;
	float SlopeMask01 = 0.0f;
	uint8 ElevationBand = 0;
	uint8 SlopeClass = 0;
	uint8 ReliefClass = 0;
	uint8 PackedCoastChannel = 0;
	uint8 PackedLandformChannel = 0;
	uint8 PackedHazardChannel = 0;
};

struct FJGTerrainChannelMetrics
{
	int32 SampleCount = 0;
	int32 DebugChannelCount = 0;
	int32 CatchmentCount = 0;
	float MaxSlopeDegrees = 0.0f;
	float MeanSlopeDegrees = 0.0f;
	float MaxReliefM = 0.0f;
	float MaxBeachMask = 0.0f;
	float MaxOceanMask = 0.0f;
	float MaxHardBlockerMask = 0.0f;
	float MaxHazardMask = 0.0f;
	int32 BeachChannelSamples = 0;
	int32 OceanChannelSamples = 0;
	int32 HardBlockerSamples = 0;
	int32 SlopeClassCounts[5] = {0, 0, 0, 0, 0};
};

struct FJGTerrainTopographicMetrics
{
	int32 ExportVerticesPerSide = 0;
	int32 ExportSampleCount = 0;
	int32 RequiredMapCount = 9;
	int32 GeneratedMapSchemaCount = 0;
	float ExportSpacingM = 0.0f;
	float MinHeightM = TNumericLimits<float>::Max();
	float MaxHeightM = TNumericLimits<float>::Lowest();
	float MeanHeightM = 0.0f;
	float MaxSlopeDegrees = 0.0f;
	float MeanSlopeDegrees = 0.0f;
	float MaxReliefM = 0.0f;
	float MaxShorelineAbsErrorM = 0.0f;
	float BeachContinuityPercent = 0.0f;
	float OceanBelowSeaPercent = 0.0f;
	float RidgeMaskMax = 0.0f;
	float GullyMaskMax = 0.0f;
	float CraterMaskMax = 0.0f;
	float HazardMaskMax = 0.0f;
	int32 ShorelineContourSamples = 0;
	int32 BeachBandSamples = 0;
	int32 BeachBandPassSamples = 0;
	int32 OceanSamples = 0;
	int32 OceanBelowSeaSamples = 0;
	int32 Contour50mBandsTouched = 0;
	int32 SlopeHistogram[5] = {0, 0, 0, 0, 0};
};

struct FJGTerrainGeneratorConfig
{
	FName GeneratorId = TEXT("JG_TERRAIN_MATH_CORE_005");
	FName VersionId = TEXT("JG_HYDROLOGY_EROSION_005_002");
	int32 Seed = 5002;
	float WorldSizeMeters = 97536.0f;
	float SeaLevelMeters = 0.0f;
	float TargetPeakMeters = 3800.0f;
	int32 SourceVerticesPerSide = 8129;
	int32 RuntimeTilesPerSideValue = 32;
	int32 ChannelCount = 15;
	bool bCanonicalCoastline = true;
	bool bDeterministicSampling = true;
	bool bRuntimeMeshBridgeEnabled = true;
	bool bTopographicEvidenceEnabled = true;
	bool bGeomorphologyCoreEnabled = true;
	bool bActiveVolcanoBounded = true;
	bool bHydrologySolverEnabled = true;
};

struct FJGTerrainGeneratorArchitectureMetrics
{
	FString ConfigFingerprint;
	int32 SourceVerticesPerSide = 0;
	int32 RuntimeTilesPerSideValue = 0;
	int32 ChannelCount = 0;
	int32 ValidationStageCount = 0;
	bool bWorldSizeMatches = false;
	bool bSeaLevelMatches = false;
	bool bSourceResolutionMatches = false;
	bool bCoastInvariantOwnedBySource = false;
	bool bRuntimeMeshBridgeEnabled = false;
	bool bTopographicEvidenceEnabled = false;
	bool bGeomorphologyCoreEnabled = false;
	bool bActiveVolcanoBounded = false;
	bool bHydrologySolverEnabled = false;
	bool bArchitectureValid = false;
};

struct FJGTerrainBatchAcceptanceMetrics
{
	FString AcceptedTerrainVersion;
	FString GeneratorFingerprint;
	int32 RuntimeFilesAccepted = 0;
	int32 RuntimeFilesExpected = 5;
	int32 RequiredLogTokenCount = 0;
	int32 LandformRegionCount = 0;
	float PeakHeightM = 0.0f;
	float VolcanoDisabledPeakM = 0.0f;
	float ActiveVolcanoContributionMaxM = 0.0f;
	float FlowAccumulationMax01 = 0.0f;
	float StreamPowerIncisionMaxM = 0.0f;
	int32 MaxStreamOrder = 0;
	int32 DrainageOutletCount = 0;
	float ShorelineErrorMaxM = 0.0f;
	float BeachContinuityPercent = 0.0f;
	float OceanBelowSeaPercent = 0.0f;
	float SeamErrorMaxM = 0.0f;
	float MaxSlopeDegrees = 0.0f;
	float HardBlockerMaskMax = 0.0f;
	bool bPeakAccepted = false;
	bool bSeaLevelAccepted = false;
	bool bBeachAccepted = false;
	bool bOceanAccepted = false;
	bool bRuntimeMeshAccepted = false;
	bool bChannelsAccepted = false;
	bool bTopographicEvidenceAccepted = false;
	bool bArchitectureAccepted = false;
	bool bVolcanoDisabledTerrainAccepted = false;
	bool bLandformRegionAuthorityAccepted = false;
	bool bHydrologyAccepted = false;
	bool bBatchAccepted = false;
};

class JUNGLEGAME_API FJungleVolcanicIslandTerrainModel
{
public:
	static constexpr float WorldSizeM = 97536.0f;
	static constexpr float HalfExtentM = WorldSizeM * 0.5f;
	static constexpr float SeaLevelM = 0.0f;
	static constexpr float MeanIslandRadiusM = 42000.0f;
	static constexpr float MaxIslandRadiusM = 44100.0f;
	static constexpr float TargetPeakHeightM = 3800.0f;
	static constexpr int32 PrimaryCatchmentCount = 23;
	static constexpr int32 LandformRegionCount = 8;
	static constexpr int32 RuntimePreviewVerticesPerSide = 129;
	static constexpr int32 SourceReferenceVerticesPerSide = 8129;
	static constexpr int32 RuntimeTilesPerSide = 32;
	static constexpr int32 RuntimeNearTileVerticesPerSide = 129;
	static constexpr int32 RuntimeMidTileVerticesPerSide = 65;
	static constexpr int32 RuntimeFarTileVerticesPerSide = 33;
	static constexpr int32 RuntimeValidationTilesPerSide = 5;

	static constexpr float SourceReferenceSpacingM = WorldSizeM / static_cast<float>(SourceReferenceVerticesPerSide - 1);
	static constexpr float RuntimeTileSizeM = WorldSizeM / static_cast<float>(RuntimeTilesPerSide);
	static constexpr float RuntimeNearTileSpacingM = RuntimeTileSizeM / static_cast<float>(RuntimeNearTileVerticesPerSide - 1);
	static constexpr float RuntimeMidTileSpacingM = RuntimeTileSizeM / static_cast<float>(RuntimeMidTileVerticesPerSide - 1);
	static constexpr float RuntimeFarTileSpacingM = RuntimeTileSizeM / static_cast<float>(RuntimeFarTileVerticesPerSide - 1);

	static FJGTerrainSample SampleTerrainMeters(float WorldXM, float WorldYM);
	static float SampleHeightMeters(float WorldXM, float WorldYM);
	static float OrganicIslandRadiusMeters(float ThetaRadians);
	static FJGTerrainMetrics BuildMetrics(int32 SamplesPerSide = RuntimePreviewVerticesPerSide);
	static FString BuildMetricsLogLine(const FJGTerrainMetrics& Metrics);
	static FJGTerrainRuntimeTileDesc BuildRuntimeTileDesc(int32 TileX, int32 TileY, int32 LodIndex, bool bCollisionEnabled);
	static void BuildRuntimeValidationTileDescs(TArray<FJGTerrainRuntimeTileDesc>& OutTiles);
	static FJGTerrainRuntimeMeshMetrics BuildRuntimeMeshMetrics(const TArray<FJGTerrainRuntimeTileDesc>& Tiles);
	static FString BuildRuntimeMeshMetricsLogLine(const FJGTerrainRuntimeMeshMetrics& Metrics);
	static FJGTerrainChannelSample SampleTerrainChannelsMeters(float WorldXM, float WorldYM, float DerivativeSpacingM = SourceReferenceSpacingM);
	static FJGTerrainChannelMetrics BuildChannelMetrics(int32 SamplesPerSide = RuntimePreviewVerticesPerSide);
	static FString BuildChannelMetricsLogLine(const FJGTerrainChannelMetrics& Metrics);
	static FJGTerrainTopographicMetrics BuildTopographicMetrics(int32 ExportVerticesPerSide = RuntimePreviewVerticesPerSide);
	static FString BuildTopographicMetricsLogLine(const FJGTerrainTopographicMetrics& Metrics);
	static FJGTerrainGeneratorConfig BuildDefaultGeneratorConfig();
	static FString BuildGeneratorConfigFingerprint(const FJGTerrainGeneratorConfig& Config);
	static FJGTerrainGeneratorArchitectureMetrics BuildGeneratorArchitectureMetrics(const FJGTerrainGeneratorConfig& Config);
	static FString BuildGeneratorArchitectureLogLine(const FJGTerrainGeneratorArchitectureMetrics& Metrics);
	static FJGTerrainBatchAcceptanceMetrics BuildBatchAcceptanceMetrics();
	static FString BuildBatchAcceptanceLogLine(const FJGTerrainBatchAcceptanceMetrics& Metrics);

private:
	static float SmoothStep(float Edge0, float Edge1, float Value);
	static float RingMask(float DistanceM, float CenterM, float HalfWidthM);
	static int32 VerticesPerSideForLod(int32 LodIndex);
};
