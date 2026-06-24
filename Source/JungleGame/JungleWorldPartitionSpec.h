#pragma once

#include "CoreMinimal.h"

/**
 * Source-of-truth constants for the large island foundation.
 *
 * These values document the intended World Partition landscape target without
 * creating editor-generated landscape assets in source control. The first
 * editor-created world should match these constants unless the design source of
 * truth changes through a later roadmap PR.
 */
struct FJungleWorldPartitionSpec
{
	static constexpr int32 HeightmapResolutionQuads = 8128;
	static constexpr int32 HeightmapResolutionVertices = 8129;
	static constexpr double LandscapeScaleMeters = 2.0;
	static constexpr double TargetWorldSizeMeters = HeightmapResolutionQuads * LandscapeScaleMeters;
	static constexpr double TargetWorldSizeKilometers = TargetWorldSizeMeters / 1000.0;
	static constexpr double OceanBoundaryBufferMeters = 512.0;

	static bool IsExpectedHeightmapResolution(int32 VertexCount)
	{
		return VertexCount == HeightmapResolutionVertices;
	}

	static FString Describe()
	{
		return FString::Printf(
			TEXT("Jungle island foundation: %d x %d vertices, %.1fm scale, %.3fkm x %.3fkm playable landscape target, ocean boundary buffer %.0fm"),
			HeightmapResolutionVertices,
			HeightmapResolutionVertices,
			LandscapeScaleMeters,
			TargetWorldSizeKilometers,
			TargetWorldSizeKilometers,
			OceanBoundaryBufferMeters);
	}
};
