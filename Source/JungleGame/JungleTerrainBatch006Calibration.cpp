#include "JungleTerrainBatch006Calibration.h"

#include "JungleVolcanicIslandTerrainModel.h"

namespace
{
float Percent(int32 Count, int32 Total)
{
	return Total > 0 ? 100.0f * static_cast<float>(Count) / static_cast<float>(Total) : 0.0f;
}

float SafeRatio(float Numerator, float Denominator)
{
	return Denominator > SMALL_NUMBER ? Numerator / Denominator : 0.0f;
}

float ShannonEntropy01(const TArray<int32>& Counts, int32 Total)
{
	if (Total <= 0)
	{
		return 0.0f;
	}

	float Entropy = 0.0f;
	int32 NonZeroBins = 0;
	for (int32 Count : Counts)
	{
		if (Count <= 0)
		{
			continue;
		}
		const float P = static_cast<float>(Count) / static_cast<float>(Total);
		Entropy -= P * FMath::Loge(P);
		++NonZeroBins;
	}

	return NonZeroBins > 1 ? FMath::Clamp(Entropy / FMath::Loge(static_cast<float>(NonZeroBins)), 0.0f, 1.0f) : 0.0f;
}

float CoefficientOfVariation(const TArray<int32>& Counts)
{
	float Sum = 0.0f;
	int32 NonZeroBins = 0;
	for (int32 Count : Counts)
	{
		if (Count > 0)
		{
			Sum += static_cast<float>(Count);
			++NonZeroBins;
		}
	}
	if (NonZeroBins <= 1)
	{
		return 0.0f;
	}

	const float Mean = Sum / static_cast<float>(NonZeroBins);
	float Variance = 0.0f;
	for (int32 Count : Counts)
	{
		if (Count > 0)
		{
			Variance += FMath::Square(static_cast<float>(Count) - Mean);
		}
	}
	Variance /= static_cast<float>(NonZeroBins);
	return SafeRatio(FMath::Sqrt(Variance), Mean);
}
}

FJGBatch006DemBenchmarkMetrics FJungleTerrainBatch006Calibration::BuildDemBenchmarkMetrics(int32 SamplesPerSide)
{
	FJGBatch006DemBenchmarkMetrics Metrics;
	Metrics.WorldSizeM = FJungleVolcanicIslandTerrainModel::WorldSizeM;
	Metrics.SourceSpacingM = FJungleVolcanicIslandTerrainModel::SourceReferenceSpacingM;

	const int32 ClampedSamplesPerSide = FMath::Max(8, SamplesPerSide);
	const float StepM = FJungleVolcanicIslandTerrainModel::WorldSizeM / static_cast<float>(ClampedSamplesPerSide - 1);
	TArray<int32> CatchmentCounts;
	CatchmentCounts.Init(0, FJungleVolcanicIslandTerrainModel::PrimaryCatchmentCount);
	TArray<float> CatchmentCosSums;
	TArray<float> CatchmentSinSums;
	CatchmentCosSums.Init(0.0f, FJungleVolcanicIslandTerrainModel::PrimaryCatchmentCount);
	CatchmentSinSums.Init(0.0f, FJungleVolcanicIslandTerrainModel::PrimaryCatchmentCount);

	static constexpr float BasinAngles[23] =
	{
		-3.04f, -2.73f, -2.48f, -2.13f, -1.86f, -1.57f, -1.20f, -0.98f,
		-0.61f, -0.33f, -0.05f, 0.29f, 0.57f, 0.92f, 1.13f, 1.46f,
		1.73f, 2.03f, 2.34f, 2.57f, 2.82f, 3.03f, 3.13f
	};
	const auto AngularDelta = [](float A, float B)
	{
		return FMath::Abs(FMath::Atan2(FMath::Sin(A - B), FMath::Cos(A - B)));
	};

	float LandHeightTotal = 0.0f;
	float VolcanoDisabledHeightTotal = 0.0f;
	float RadialSuppressionTotal = 0.0f;
	float RegionWeightTotal = 0.0f;
	float RidgeGullyAngularLockTotal = 0.0f;
	float RidgeGullyAngularLockWeight = 0.0f;
	int32 ActiveVolcanoSamples = 0;
	int32 CoastalLowlandSamples = 0;
	int32 HighlandSamples = 0;
	int32 GentleSlopeSamples = 0;
	int32 ModerateSlopeSamples = 0;
	int32 SteepSlopeSamples = 0;
	int32 DrainageSamples = 0;

	for (int32 Y = 0; Y < ClampedSamplesPerSide; ++Y)
	{
		for (int32 X = 0; X < ClampedSamplesPerSide; ++X)
		{
			const float WorldXM = -FJungleVolcanicIslandTerrainModel::HalfExtentM + static_cast<float>(X) * StepM;
			const float WorldYM = -FJungleVolcanicIslandTerrainModel::HalfExtentM + static_cast<float>(Y) * StepM;
			const FVector2D FromMassifM(WorldXM + 1320.0f, WorldYM - 1080.0f);
			const float MassifTheta = FMath::Atan2(FromMassifM.Y, FromMassifM.X);
			const FJGTerrainChannelSample Channels = FJungleVolcanicIslandTerrainModel::SampleTerrainChannelsMeters(WorldXM, WorldYM, FJungleVolcanicIslandTerrainModel::SourceReferenceSpacingM);
			const FJGTerrainSample& Sample = Channels.Terrain;
			++Metrics.SampleCount;

			if (Sample.OceanMask > 0.5f || Sample.HeightM <= FJungleVolcanicIslandTerrainModel::SeaLevelM)
			{
				continue;
			}

			++Metrics.LandSampleCount;
			Metrics.PeakHeightM = FMath::Max(Metrics.PeakHeightM, Sample.HeightM);
			Metrics.VolcanoDisabledPeakM = FMath::Max(Metrics.VolcanoDisabledPeakM, Sample.VolcanoDisabledHeightM);
			LandHeightTotal += Sample.HeightM;
			VolcanoDisabledHeightTotal += FMath::Max(0.0f, Sample.VolcanoDisabledHeightM);
			RadialSuppressionTotal += Sample.RadialArtifactSuppressionMask;
			RegionWeightTotal += Sample.LandformRegionWeight;

			const float ActiveVolcanoMask = FMath::Max(Sample.ActiveConeMask, FMath::Max(Sample.CraterMask, FMath::Max(Sample.RiftLineMask, Sample.LavaFlowMask)));
			if (ActiveVolcanoMask > 0.25f)
			{
				++ActiveVolcanoSamples;
			}
			if (Sample.CoastalLowlandMask > 0.2f || (Sample.HeightM <= 150.0f && Sample.SignedShorelineDistanceM <= 5000.0f))
			{
				++CoastalLowlandSamples;
			}
			if (Sample.HeightM >= 600.0f && ActiveVolcanoMask < 0.25f)
			{
				++HighlandSamples;
			}
			if (Channels.SlopeDegrees < 10.0f)
			{
				++GentleSlopeSamples;
			}
			else if (Channels.SlopeDegrees < 28.0f)
			{
				++ModerateSlopeSamples;
			}
			else
			{
				++SteepSlopeSamples;
			}
			if (Sample.TrunkStreamMask > 0.35f || Sample.TributaryStreamMask > 0.35f || Sample.FlowAccumulation01 > 0.55f)
			{
				++DrainageSamples;
			}
			if (CatchmentCounts.IsValidIndex(Sample.CatchmentId))
			{
				++CatchmentCounts[Sample.CatchmentId];
				CatchmentCosSums[Sample.CatchmentId] += FMath::Cos(MassifTheta);
				CatchmentSinSums[Sample.CatchmentId] += FMath::Sin(MassifTheta);

				const float RidgeGullyMask = FMath::Max(Sample.RidgeMask, Sample.GullyMask);
				if (RidgeGullyMask > 0.25f && Sample.CatchmentId < UE_ARRAY_COUNT(BasinAngles))
				{
					const float BasinLock = 1.0f - FMath::Clamp(AngularDelta(MassifTheta, BasinAngles[Sample.CatchmentId]) / 0.30f, 0.0f, 1.0f);
					RidgeGullyAngularLockTotal += BasinLock * RidgeGullyMask;
					RidgeGullyAngularLockWeight += RidgeGullyMask;
				}
			}
		}
	}

	Metrics.IslandHypsometricIntegral = SafeRatio(SafeRatio(LandHeightTotal, static_cast<float>(Metrics.LandSampleCount)), Metrics.PeakHeightM);
	Metrics.VolcanoDisabledHypsometricIntegral = SafeRatio(SafeRatio(VolcanoDisabledHeightTotal, static_cast<float>(Metrics.LandSampleCount)), Metrics.VolcanoDisabledPeakM);
	Metrics.ActiveVolcanoLandAreaPercent = Percent(ActiveVolcanoSamples, Metrics.LandSampleCount);
	Metrics.CoastalLowlandLandAreaPercent = Percent(CoastalLowlandSamples, Metrics.LandSampleCount);
	Metrics.HighlandLandAreaPercent = Percent(HighlandSamples, Metrics.LandSampleCount);
	Metrics.GentleSlopePercent = Percent(GentleSlopeSamples, Metrics.LandSampleCount);
	Metrics.ModerateSlopePercent = Percent(ModerateSlopeSamples, Metrics.LandSampleCount);
	Metrics.SteepSlopePercent = Percent(SteepSlopeSamples, Metrics.LandSampleCount);
	Metrics.DrainageDensityProxy = SafeRatio(static_cast<float>(DrainageSamples), static_cast<float>(Metrics.LandSampleCount));
	Metrics.CatchmentAreaCoefficientOfVariation = CoefficientOfVariation(CatchmentCounts);
	Metrics.CatchmentEntropy01 = ShannonEntropy01(CatchmentCounts, Metrics.LandSampleCount);
	float AngularConcentrationTotal = 0.0f;
	int32 AngularConcentrationBins = 0;
	for (int32 CatchmentIndex = 0; CatchmentIndex < CatchmentCounts.Num(); ++CatchmentIndex)
	{
		if (CatchmentCounts[CatchmentIndex] > 0)
		{
			const float Count = static_cast<float>(CatchmentCounts[CatchmentIndex]);
			AngularConcentrationTotal += FMath::Sqrt(FMath::Square(CatchmentCosSums[CatchmentIndex]) + FMath::Square(CatchmentSinSums[CatchmentIndex])) / Count;
			++AngularConcentrationBins;
		}
	}
	Metrics.CatchmentAngularConcentrationScore = SafeRatio(AngularConcentrationTotal, static_cast<float>(AngularConcentrationBins));
	const bool bHasRidgeGullyAngularLockSamples = RidgeGullyAngularLockWeight > SMALL_NUMBER;
	Metrics.RidgeGullyAngularLockScore = bHasRidgeGullyAngularLockSamples ? RidgeGullyAngularLockTotal / RidgeGullyAngularLockWeight : 1.0f;
	const float MeanRadialSuppression = SafeRatio(RadialSuppressionTotal, static_cast<float>(Metrics.LandSampleCount));
	const float MeanRegionWeight = SafeRatio(RegionWeightTotal, static_cast<float>(Metrics.LandSampleCount));
	Metrics.RadialAlignmentArtifactScore = FMath::Clamp(1.0f - (Metrics.CatchmentEntropy01 * 0.34f + Metrics.CatchmentAreaCoefficientOfVariation * 0.18f + MeanRadialSuppression * 0.26f + MeanRegionWeight * 0.18f), 0.0f, 1.0f);
	Metrics.ContourIrregularityProxy = FMath::Clamp(Metrics.CatchmentAreaCoefficientOfVariation * 0.45f + MeanRegionWeight * 0.35f + MeanRadialSuppression * 0.20f, 0.0f, 1.0f);
	Metrics.VolcanoDominancePercent = SafeRatio(Metrics.PeakHeightM - Metrics.VolcanoDisabledPeakM, Metrics.PeakHeightM) * 100.0f;

	Metrics.bWorldScaleAccepted = FMath::IsNearlyEqual(Metrics.WorldSizeM, 97536.0f, 0.1f) && Metrics.SourceSpacingM <= 12.1f;
	Metrics.bPeakEnvelopeAccepted = Metrics.PeakHeightM >= 2400.0f && Metrics.PeakHeightM <= 4050.0f;
	Metrics.bVolcanoFootprintAccepted = Metrics.ActiveVolcanoLandAreaPercent >= 1.0f && Metrics.ActiveVolcanoLandAreaPercent <= 20.0f && Metrics.VolcanoDominancePercent <= 45.0f;
	Metrics.bHypsometryAccepted = Metrics.IslandHypsometricIntegral >= 0.25f && Metrics.IslandHypsometricIntegral <= 0.58f && Metrics.VolcanoDisabledHypsometricIntegral >= 0.20f && Metrics.VolcanoDisabledHypsometricIntegral <= 0.55f;
	Metrics.bSlopeEnvelopeAccepted = Metrics.GentleSlopePercent >= 15.0f && Metrics.SteepSlopePercent <= 45.0f;
	Metrics.bCatchmentDiversityAccepted = Metrics.CatchmentEntropy01 >= 0.72f && Metrics.CatchmentAreaCoefficientOfVariation >= 0.10f;
	Metrics.bRadialArtifactAccepted = Metrics.RadialAlignmentArtifactScore <= 0.62f && Metrics.ContourIrregularityProxy >= 0.20f;
	Metrics.bCatchmentGeometryAccepted = Metrics.CatchmentAngularConcentrationScore <= 0.82f;
	Metrics.bRidgeGullyGeometryAccepted = bHasRidgeGullyAngularLockSamples && Metrics.RidgeGullyAngularLockScore <= 0.38f;
	Metrics.bAccepted = Metrics.bWorldScaleAccepted && Metrics.bPeakEnvelopeAccepted && Metrics.bVolcanoFootprintAccepted && Metrics.bHypsometryAccepted && Metrics.bSlopeEnvelopeAccepted && Metrics.bCatchmentDiversityAccepted && Metrics.bRadialArtifactAccepted && Metrics.bCatchmentGeometryAccepted && Metrics.bRidgeGullyGeometryAccepted;
	return Metrics;
}

FString FJungleTerrainBatch006Calibration::BuildDemBenchmarkLogLine(const FJGBatch006DemBenchmarkMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=%s samples=%d land_samples=%d world_m=%.0f source_spacing_m=%.2f reference_families=%d peak_m=%.1f volcano_disabled_peak_m=%.1f hypsometry=%.3f volcano_disabled_hypsometry=%.3f active_volcano_land_pct=%.2f coastal_lowland_pct=%.2f highland_pct=%.2f slope_pct_gentle_moderate_steep=%.1f/%.1f/%.1f drainage_density_proxy=%.3f catchment_cv=%.3f catchment_entropy=%.3f radial_artifact_score=%.3f contour_irregularity=%.3f catchment_angular_concentration=%.3f ridge_gully_angular_lock=%.3f volcano_dominance_pct=%.2f accepted=%s"),
		*Metrics.CalibrationId.ToString(),
		Metrics.SampleCount,
		Metrics.LandSampleCount,
		Metrics.WorldSizeM,
		Metrics.SourceSpacingM,
		Metrics.ReferenceFamilyCount,
		Metrics.PeakHeightM,
		Metrics.VolcanoDisabledPeakM,
		Metrics.IslandHypsometricIntegral,
		Metrics.VolcanoDisabledHypsometricIntegral,
		Metrics.ActiveVolcanoLandAreaPercent,
		Metrics.CoastalLowlandLandAreaPercent,
		Metrics.HighlandLandAreaPercent,
		Metrics.GentleSlopePercent,
		Metrics.ModerateSlopePercent,
		Metrics.SteepSlopePercent,
		Metrics.DrainageDensityProxy,
		Metrics.CatchmentAreaCoefficientOfVariation,
		Metrics.CatchmentEntropy01,
		Metrics.RadialAlignmentArtifactScore,
		Metrics.ContourIrregularityProxy,
		Metrics.CatchmentAngularConcentrationScore,
		Metrics.RidgeGullyAngularLockScore,
		Metrics.VolcanoDominancePercent,
		Metrics.bAccepted ? TEXT("true") : TEXT("false"));
}

FJGBatch006OfflineBridgeMetrics FJungleTerrainBatch006Calibration::BuildOfflineBridgeMetrics()
{
	FJGBatch006OfflineBridgeMetrics Metrics;
	const FJGTerrainGeneratorConfig Config = FJungleVolcanicIslandTerrainModel::BuildDefaultGeneratorConfig();
	Metrics.SourceFingerprint = FJungleVolcanicIslandTerrainModel::BuildGeneratorConfigFingerprint(Config);
	Metrics.SourceVerticesPerSide = FJungleVolcanicIslandTerrainModel::SourceReferenceVerticesPerSide;
	Metrics.RuntimeTilesPerSide = FJungleVolcanicIslandTerrainModel::RuntimeTilesPerSide;
	Metrics.RequiredManifestFieldCount = 29;
	Metrics.bCanonicalSourceOwned = Config.bCanonicalCoastline && Config.bDeterministicSampling;
	Metrics.bRuntimeSampleParity = Config.bRuntimeMeshBridgeEnabled && Metrics.RuntimeTilesPerSide == FJungleVolcanicIslandTerrainModel::RuntimeTilesPerSide;
	Metrics.bPreviewExporterParity = Config.bTopographicEvidenceEnabled && Metrics.SourceVerticesPerSide == FJungleVolcanicIslandTerrainModel::SourceReferenceVerticesPerSide;
	Metrics.bBridgeAccepted = Metrics.bCanonicalSourceOwned && Metrics.bRuntimeSampleParity && Metrics.bPreviewExporterParity && Metrics.bHeavyArtifactsKeptOutOfGit;
	return Metrics;
}

FString FJungleTerrainBatch006Calibration::BuildOfflineBridgeLogLine(const FJGBatch006OfflineBridgeMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=%s fingerprint=%s source_vertices=%d runtime_tiles_per_side=%d tracked_preview_run=%d manifest_fields=%d canonical_source_owned=%s runtime_sample_parity=%s preview_exporter_parity=%s heavy_artifacts_git_ignored=%s accepted=%s"),
		*Metrics.BridgeId.ToString(),
		*Metrics.SourceFingerprint,
		Metrics.SourceVerticesPerSide,
		Metrics.RuntimeTilesPerSide,
		Metrics.TrackedPreviewRunIndex,
		Metrics.RequiredManifestFieldCount,
		Metrics.bCanonicalSourceOwned ? TEXT("true") : TEXT("false"),
		Metrics.bRuntimeSampleParity ? TEXT("true") : TEXT("false"),
		Metrics.bPreviewExporterParity ? TEXT("true") : TEXT("false"),
		Metrics.bHeavyArtifactsKeptOutOfGit ? TEXT("true") : TEXT("false"),
		Metrics.bBridgeAccepted ? TEXT("true") : TEXT("false"));
}

FJGBatch006PreviewValidationMetrics FJungleTerrainBatch006Calibration::BuildPreviewValidationMetrics(int32 SamplesPerSide)
{
	FJGBatch006PreviewValidationMetrics Metrics;
	const FJGTerrainMetrics TerrainMetrics = FJungleVolcanicIslandTerrainModel::BuildMetrics(SamplesPerSide);
	const FJGTerrainTopographicMetrics TopographicMetrics = FJungleVolcanicIslandTerrainModel::BuildTopographicMetrics(SamplesPerSide);
	const FJGBatch006DemBenchmarkMetrics DemMetrics = BuildDemBenchmarkMetrics(SamplesPerSide);
	Metrics.WorldSizeM = FJungleVolcanicIslandTerrainModel::WorldSizeM;
	Metrics.PixelSpacing1024M = FJungleVolcanicIslandTerrainModel::WorldSizeM / 1023.0f;
	Metrics.ShorelineErrorMaxM = TerrainMetrics.MaxShorelineAbsErrorM;
	Metrics.BeachContinuityPercent = TerrainMetrics.BeachContinuityPercent;
	Metrics.OceanBelowSeaPercent = TopographicMetrics.OceanBelowSeaPercent;
	Metrics.SquareEdgeOceanViolationCount = static_cast<float>(TerrainMetrics.SquareEdgeOceanViolationCount);
	Metrics.ActiveVolcanoLandAreaPercent = DemMetrics.ActiveVolcanoLandAreaPercent;
	Metrics.RadialAlignmentArtifactScore = DemMetrics.RadialAlignmentArtifactScore;
	Metrics.CatchmentAreaCoefficientOfVariation = DemMetrics.CatchmentAreaCoefficientOfVariation;
	Metrics.CatchmentEntropy01 = DemMetrics.CatchmentEntropy01;
	Metrics.CatchmentAngularConcentrationScore = DemMetrics.CatchmentAngularConcentrationScore;
	Metrics.RidgeGullyAngularLockScore = DemMetrics.RidgeGullyAngularLockScore;
	Metrics.bCoastAccepted = Metrics.ShorelineErrorMaxM <= 0.25f && Metrics.BeachContinuityPercent >= 99.0f && Metrics.OceanBelowSeaPercent >= 99.0f && Metrics.SquareEdgeOceanViolationCount <= 0.0f;
	Metrics.bMorphologyAccepted = Metrics.ActiveVolcanoLandAreaPercent <= 20.0f && Metrics.RadialAlignmentArtifactScore <= 0.62f && Metrics.CatchmentAreaCoefficientOfVariation >= 0.10f && Metrics.CatchmentEntropy01 >= 0.72f && Metrics.CatchmentAngularConcentrationScore <= 0.82f && Metrics.RidgeGullyAngularLockScore <= 0.38f;
	Metrics.bPreviewEvidenceAccepted = Metrics.bCoastAccepted && Metrics.bMorphologyAccepted;
	return Metrics;
}

FString FJungleTerrainBatch006Calibration::BuildPreviewValidationLogLine(const FJGBatch006PreviewValidationMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=%s expected_outputs=%d manifest_metrics=%d world_m=%.0f pixel_spacing_1024_m=%.2f shoreline_error_max_m=%.3f beach_continuity_pct=%.1f ocean_below_sea_pct=%.1f square_edge_violations=%.0f active_volcano_land_pct=%.2f radial_artifact_score=%.3f catchment_cv=%.3f catchment_entropy=%.3f catchment_angular_concentration=%.3f ridge_gully_angular_lock=%.3f coast_ok=%s morphology_ok=%s accepted=%s"),
		*Metrics.PreviewSuiteId.ToString(),
		Metrics.ExpectedPreviewOutputCount,
		Metrics.RequiredManifestMetricCount,
		Metrics.WorldSizeM,
		Metrics.PixelSpacing1024M,
		Metrics.ShorelineErrorMaxM,
		Metrics.BeachContinuityPercent,
		Metrics.OceanBelowSeaPercent,
		Metrics.SquareEdgeOceanViolationCount,
		Metrics.ActiveVolcanoLandAreaPercent,
		Metrics.RadialAlignmentArtifactScore,
		Metrics.CatchmentAreaCoefficientOfVariation,
		Metrics.CatchmentEntropy01,
		Metrics.CatchmentAngularConcentrationScore,
		Metrics.RidgeGullyAngularLockScore,
		Metrics.bCoastAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bMorphologyAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bPreviewEvidenceAccepted ? TEXT("true") : TEXT("false"));
}

FJGBatch006AcceptanceMetrics FJungleTerrainBatch006Calibration::BuildAcceptanceMetrics()
{
	FJGBatch006AcceptanceMetrics Metrics;
	const FJGBatch006DemBenchmarkMetrics DemMetrics = BuildDemBenchmarkMetrics();
	const FJGBatch006OfflineBridgeMetrics BridgeMetrics = BuildOfflineBridgeMetrics();
	const FJGBatch006PreviewValidationMetrics PreviewMetrics = BuildPreviewValidationMetrics();
	TArray<FJGTerrainRuntimeTileDesc> RuntimeTiles;
	FJungleVolcanicIslandTerrainModel::BuildRuntimeValidationTileDescs(RuntimeTiles);
	const FJGTerrainRuntimeMeshMetrics RuntimeMetrics = FJungleVolcanicIslandTerrainModel::BuildRuntimeMeshMetrics(RuntimeTiles);
	Metrics.SourceFingerprint = BridgeMetrics.SourceFingerprint;
	Metrics.bDemCalibrationAccepted = DemMetrics.bAccepted;
	Metrics.bOfflineBridgeAccepted = BridgeMetrics.bBridgeAccepted;
	Metrics.bPreviewValidationAccepted = PreviewMetrics.bPreviewEvidenceAccepted;
	Metrics.bCoastAccepted = PreviewMetrics.bCoastAccepted;
	Metrics.bScaleAccepted = DemMetrics.bWorldScaleAccepted;
	Metrics.bTerrainMorphologyAccepted = DemMetrics.bRadialArtifactAccepted && DemMetrics.bCatchmentDiversityAccepted && DemMetrics.bVolcanoFootprintAccepted && DemMetrics.bCatchmentGeometryAccepted && DemMetrics.bRidgeGullyGeometryAccepted;
	Metrics.bRuntimeShellAccepted = RuntimeMetrics.TileCount > 0 && FJungleVolcanicIslandTerrainModel::RuntimeTilesPerSide >= 32 && FJungleVolcanicIslandTerrainModel::RuntimePreviewVerticesPerSide >= 129 && RuntimeMetrics.MaxAdjacentSeamAbsErrorM <= 0.001f;
	Metrics.bBatchAccepted = Metrics.bDemCalibrationAccepted && Metrics.bOfflineBridgeAccepted && Metrics.bPreviewValidationAccepted && Metrics.bCoastAccepted && Metrics.bScaleAccepted && Metrics.bTerrainMorphologyAccepted && Metrics.bRuntimeShellAccepted;
	return Metrics;
}

FString FJungleTerrainBatch006Calibration::BuildAcceptanceLogLine(const FJGBatch006AcceptanceMetrics& Metrics)
{
	return FString::Printf(
		TEXT("id=%s fingerprint=%s runtime_files=%d/%d log_tokens=%d dem_calibration_ok=%s offline_bridge_ok=%s preview_validation_ok=%s coast_ok=%s scale_ok=%s morphology_ok=%s runtime_shell_ok=%s batch_accepted=%s"),
		*Metrics.AcceptanceId.ToString(),
		*Metrics.SourceFingerprint,
		Metrics.RuntimeFilesAccepted,
		Metrics.RuntimeFilesExpected,
		Metrics.RequiredLogTokenCount,
		Metrics.bDemCalibrationAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bOfflineBridgeAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bPreviewValidationAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bCoastAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bScaleAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bTerrainMorphologyAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bRuntimeShellAccepted ? TEXT("true") : TEXT("false"),
		Metrics.bBatchAccepted ? TEXT("true") : TEXT("false"));
}
