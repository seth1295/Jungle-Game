#pragma once

#include "CoreMinimal.h"
#include "JungleWorldPartitionSpec.h"

/**
 * Deterministic terrain-source contract for the large island landscape.
 *
 * This is a scaffold for generation/import tools. It defines the expected data
 * products without committing a final heightfield or editor-generated assets.
 */
struct FJungleTerrainSourceSpec
{
	static constexpr int32 HeightSamplesPerSide = FJungleWorldPartitionSpec::HeightmapResolutionVertices;
	static constexpr int32 DerivedSamplesPerSide = FJungleWorldPartitionSpec::HeightmapResolutionVertices;
	static constexpr double SampleSpacingMeters = FJungleWorldPartitionSpec::LandscapeScaleMeters;

	static constexpr const TCHAR* HeightLayerName = TEXT("height_meters");
	static constexpr const TCHAR* SlopeLayerName = TEXT("slope_degrees");
	static constexpr const TCHAR* AspectLayerName = TEXT("aspect_degrees");
	static constexpr const TCHAR* FlowLayerName = TEXT("flow_accumulation");
	static constexpr const TCHAR* RidgeValleyLayerName = TEXT("ridge_valley_index");
	static constexpr const TCHAR* CoastDistanceLayerName = TEXT("coast_distance_meters");

	static bool MatchesFoundation(int32 SamplesPerSide, double SpacingMeters)
	{
		return SamplesPerSide == HeightSamplesPerSide && FMath::IsNearlyEqual(SpacingMeters, SampleSpacingMeters);
	}

	static TArray<FString> RequiredLayerNames()
	{
		return {
			HeightLayerName,
			SlopeLayerName,
			AspectLayerName,
			FlowLayerName,
			RidgeValleyLayerName,
			CoastDistanceLayerName,
		};
	}
};
