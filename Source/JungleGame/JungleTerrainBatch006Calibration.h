#pragma once

#include "CoreMinimal.h"

struct FJGBatch006DemBenchmarkMetrics
{
	FName CalibrationId = TEXT("JG_DEM_CALIBRATION_006_001");
	int32 SampleCount = 0;
	int32 LandSampleCount = 0;
	int32 ReferenceFamilyCount = 6;
	float WorldSizeM = 0.0f;
	float SourceSpacingM = 0.0f;
	float PeakHeightM = 0.0f;
	float VolcanoDisabledPeakM = 0.0f;
	float IslandHypsometricIntegral = 0.0f;
	float VolcanoDisabledHypsometricIntegral = 0.0f;
	float ActiveVolcanoLandAreaPercent = 0.0f;
	float CoastalLowlandLandAreaPercent = 0.0f;
	float HighlandLandAreaPercent = 0.0f;
	float GentleSlopePercent = 0.0f;
	float ModerateSlopePercent = 0.0f;
	float SteepSlopePercent = 0.0f;
	float DrainageDensityProxy = 0.0f;
	float CatchmentAreaCoefficientOfVariation = 0.0f;
	float CatchmentEntropy01 = 0.0f;
	float RadialAlignmentArtifactScore = 1.0f;
	float ContourIrregularityProxy = 0.0f;
	float CatchmentAngularConcentrationScore = 1.0f;
	float RidgeGullyAngularLockScore = 1.0f;
	float VolcanoDominancePercent = 0.0f;
	bool bWorldScaleAccepted = false;
	bool bPeakEnvelopeAccepted = false;
	bool bVolcanoFootprintAccepted = false;
	bool bHypsometryAccepted = false;
	bool bSlopeEnvelopeAccepted = false;
	bool bCatchmentDiversityAccepted = false;
	bool bRadialArtifactAccepted = false;
	bool bCatchmentGeometryAccepted = false;
	bool bRidgeGullyGeometryAccepted = false;
	bool bAccepted = false;
};

struct FJGBatch006OfflineBridgeMetrics
{
	FName BridgeId = TEXT("JG_OFFLINE_RUNTIME_BRIDGE_006_002");
	FString SourceFingerprint;
	int32 SourceVerticesPerSide = 0;
	int32 RuntimeTilesPerSide = 0;
	int32 RequiredManifestFieldCount = 0;
	int32 TrackedPreviewRunIndex = 3;
	bool bCanonicalSourceOwned = false;
	bool bRuntimeSampleParity = false;
	bool bPreviewExporterParity = false;
	bool bHeavyArtifactsKeptOutOfGit = true;
	bool bBridgeAccepted = false;
};

struct FJGBatch006PreviewValidationMetrics
{
	FName PreviewSuiteId = TEXT("JG_MORPHOLOGY_PREVIEW_SUITE_006_003");
	int32 ExpectedPreviewOutputCount = 8;
	int32 RequiredManifestMetricCount = 29;
	float WorldSizeM = 0.0f;
	float PixelSpacing1024M = 0.0f;
	float ShorelineErrorMaxM = 0.0f;
	float BeachContinuityPercent = 0.0f;
	float OceanBelowSeaPercent = 0.0f;
	float SquareEdgeOceanViolationCount = 0.0f;
	float ActiveVolcanoLandAreaPercent = 0.0f;
	float RadialAlignmentArtifactScore = 1.0f;
	float CatchmentAreaCoefficientOfVariation = 0.0f;
	float CatchmentEntropy01 = 0.0f;
	float CatchmentAngularConcentrationScore = 1.0f;
	float RidgeGullyAngularLockScore = 1.0f;
	bool bCoastAccepted = false;
	bool bMorphologyAccepted = false;
	bool bPreviewEvidenceAccepted = false;
};

struct FJGBatch006AcceptanceMetrics
{
	FName AcceptanceId = TEXT("JG_TERRAIN_BATCH006_ACCEPTANCE_006_005");
	FString SourceFingerprint;
	int32 RuntimeFilesAccepted = 5;
	int32 RuntimeFilesExpected = 5;
	int32 RequiredLogTokenCount = 11;
	bool bDemCalibrationAccepted = false;
	bool bOfflineBridgeAccepted = false;
	bool bPreviewValidationAccepted = false;
	bool bCoastAccepted = false;
	bool bScaleAccepted = false;
	bool bTerrainMorphologyAccepted = false;
	bool bRuntimeShellAccepted = false;
	bool bBatchAccepted = false;
};

class JUNGLEGAME_API FJungleTerrainBatch006Calibration
{
public:
	static FJGBatch006DemBenchmarkMetrics BuildDemBenchmarkMetrics(int32 SamplesPerSide = 129);
	static FString BuildDemBenchmarkLogLine(const FJGBatch006DemBenchmarkMetrics& Metrics);
	static FJGBatch006OfflineBridgeMetrics BuildOfflineBridgeMetrics();
	static FString BuildOfflineBridgeLogLine(const FJGBatch006OfflineBridgeMetrics& Metrics);
	static FJGBatch006PreviewValidationMetrics BuildPreviewValidationMetrics(int32 SamplesPerSide = 129);
	static FString BuildPreviewValidationLogLine(const FJGBatch006PreviewValidationMetrics& Metrics);
	static FJGBatch006AcceptanceMetrics BuildAcceptanceMetrics();
	static FString BuildAcceptanceLogLine(const FJGBatch006AcceptanceMetrics& Metrics);
};
