#pragma once

#include "CoreMinimal.h"

/** Developer-only evidence categories for world-system PR validation. */
enum class EJGWorldValidationCategory : uint8
{
	TerrainForm,
	TerrainContinuity,
	BiomeMask,
	PCGOutput,
	TraversalResistance,
	RouteClass,
	NoMapReadability,
	WeatherNightImpact,
	WorldPartitionContinuity,
	PackageSmoke,
	RepoHygiene,
};

enum class EJGValidationArtifactType : uint8
{
	EditorDebugScreenshot,
	FirstPersonScreenshot,
	MaskSampleSummary,
	PCGAttributeSummary,
	RuntimeSmokeNote,
	PackageSmokeNote,
	RepoHygieneNote,
	ExternalManifest,
};

enum class EJGTerrainLandformClass : uint8
{
	Coast,
	Creek,
	Ridge,
	Saddle,
	Basin,
	Gorge,
	Lowland,
	CanopyGap,
	SlopeBreak,
};

enum class EJGWorldTraversalClass : uint8
{
	Open,
	SlowVegetation,
	WetSlow,
	SteepSlow,
	Scramble,
	Blocked,
	FalseAffordance,
};

enum class EJGWorldRouteClass : uint8
{
	CoastRoute,
	CreekRoute,
	RidgeRoute,
	SaddlePass,
	BasinExit,
	GorgeRoute,
	LowlandPush,
	CanopyGapRoute,
	SlopeBreakTraverse,
	BlockedRoute,
	FalseAffordance,
};

enum class EJGWorldBiomeClass : uint8
{
	Coast,
	RiparianCreek,
	LowlandRainforest,
	RidgeForest,
	BasinWetland,
	Gorge,
	CanopyGap,
	DisturbedObservationArea,
};

enum class EJGWorldDebugView : uint8
{
	TerrainHeightBands,
	TerrainSlope,
	TerrainSlopeBreaks,
	TerrainDrainageBias,
	TerrainLandformClass,
	LandscapeComponentGrid,
	ComponentSeamRisk,
	BiomeDominantClass,
	BiomeMaskWeights,
	BiomeTransitionBands,
	WetnessMask,
	CanopyDensity,
	PCGDensity,
	PCGSpawnClass,
	PCGExclusionZones,
	TraversalResistance,
	TraversalClass,
	RouteClass,
	RouteCostGradient,
	RouteCandidateGraph,
	FalseAffordanceMarkers,
	PlayerStepProbe,
	NightVisibilityBands,
	RainVisibilityImpact,
	WetTraversalDelta,
	LandmarkSilhouetteCheck,
	ReadabilityOcclusion,
	WorldPartitionCellState,
	StreamingBoundaryContinuity,
	CellEdgePCGContinuity,
	CellEdgeTraversalContinuity,
	HLODContext,
};

enum class EJGValidationCondition : uint8
{
	ClearDay,
	Rain,
	WetPostRain,
	Night,
	Fog,
	PackageRuntime,
};

enum class EJGPackageSmokeResult : uint8
{
	NotRun,
	Pass,
	Fail,
	Blocked,
};

struct FJGWorldValidationArtifactRef
{
	FName ArtifactId;
	EJGValidationArtifactType Type = EJGValidationArtifactType::ExternalManifest;
	FName StableName;
	FName ExternalManifestId;
	bool bCommittedToSource = false;
	bool bDeveloperOnly = true;
};

struct FJGWorldValidationCategoryContract
{
	EJGWorldValidationCategory Category = EJGWorldValidationCategory::TerrainForm;
	FName StableId;
	EJGValidationArtifactType MinimumArtifactType = EJGValidationArtifactType::EditorDebugScreenshot;
	bool bRequiresFirstPersonEvidence = true;
	bool bRequiresDebugEvidence = true;
	bool bDeveloperOnly = true;
};

struct FJGFirstPersonScreenshotSpec
{
	FName ScreenshotId;
	EJGTerrainLandformClass Landform = EJGTerrainLandformClass::Ridge;
	EJGValidationCondition Condition = EJGValidationCondition::ClearDay;
	FName ViewIntent;
	bool bPlayerEyeHeight = true;
	bool bRequiresNoDebugOverlay = true;
	bool bRequiresNoPlayerHud = true;
};

struct FJGWorldDebugViewSpec
{
	EJGWorldDebugView View = EJGWorldDebugView::TerrainHeightBands;
	FName StableName;
	EJGWorldValidationCategory Category = EJGWorldValidationCategory::TerrainForm;
	bool bEditorOnly = true;
	bool bAllowedInRuntimePackage = false;
};

struct FJGPackageSmokeEvidence
{
	FName SmokeId;
	FName MapName;
	FName RouteId;
	FName BuildConfiguration;
	EJGPackageSmokeResult Result = EJGPackageSmokeResult::NotRun;
	bool bDebugExposed = false;
	bool bWorldPartitionIssueObserved = false;
	bool bWeatherNightIncluded = false;
	FName BlockerReason;
};

struct FJGRepoHygieneEvidence
{
	bool bGeneratedScreenshotsCommitted = false;
	bool bSavedIntermediateOrDDCCommitted = false;
	bool bPackageOutputCommitted = false;
	bool bArtifactManifestIncluded = false;
	bool bGitStatusChecked = false;
	bool bDiffStatReviewed = false;

	bool PassesRequiredHygiene() const
	{
		return !bGeneratedScreenshotsCommitted
			&& !bSavedIntermediateOrDDCCommitted
			&& !bPackageOutputCommitted
			&& bGitStatusChecked
			&& bDiffStatReviewed;
	}
};

struct FJGWorldValidationNoteSpec
{
	FName NoteId;
	FName PRLabel;
	FName CommitShortSha;
	TArray<EJGWorldValidationCategory> CategoriesCovered;
	TArray<FJGWorldValidationArtifactRef> ArtifactRefs;
	FJGPackageSmokeEvidence PackageSmoke;
	FJGRepoHygieneEvidence RepoHygiene;
	bool bDeveloperOnlyEvidence = true;

	bool HasMinimumEvidence() const
	{
		return CategoriesCovered.Num() > 0 && RepoHygiene.PassesRequiredHygiene();
	}
};

struct FJGWorldValidationEvidenceSpec
{
	static constexpr int32 EvidenceSpecVersion = 1;
	static constexpr int32 MinimumConnectedFirstPersonViews = 3;

	static bool CommitsGeneratedBinaryEvidence()
	{
		return false;
	}

	static bool AllowsDebugViewsInPlayerPackage()
	{
		return false;
	}

	static TArray<FJGWorldValidationCategoryContract> CategoryContracts()
	{
		return {
			{ EJGWorldValidationCategory::TerrainForm, TEXT("WV_TerrainForm"), EJGValidationArtifactType::EditorDebugScreenshot, true, true, true },
			{ EJGWorldValidationCategory::TerrainContinuity, TEXT("WV_TerrainContinuity"), EJGValidationArtifactType::EditorDebugScreenshot, true, true, true },
			{ EJGWorldValidationCategory::BiomeMask, TEXT("WV_BiomeMask"), EJGValidationArtifactType::MaskSampleSummary, false, true, true },
			{ EJGWorldValidationCategory::PCGOutput, TEXT("WV_PCGOutput"), EJGValidationArtifactType::PCGAttributeSummary, false, true, true },
			{ EJGWorldValidationCategory::TraversalResistance, TEXT("WV_TraversalResistance"), EJGValidationArtifactType::EditorDebugScreenshot, true, true, true },
			{ EJGWorldValidationCategory::RouteClass, TEXT("WV_RouteClass"), EJGValidationArtifactType::FirstPersonScreenshot, true, true, true },
			{ EJGWorldValidationCategory::NoMapReadability, TEXT("WV_NoMapReadability"), EJGValidationArtifactType::FirstPersonScreenshot, true, false, true },
			{ EJGWorldValidationCategory::WeatherNightImpact, TEXT("WV_WeatherNightImpact"), EJGValidationArtifactType::FirstPersonScreenshot, true, true, true },
			{ EJGWorldValidationCategory::WorldPartitionContinuity, TEXT("WV_WorldPartitionContinuity"), EJGValidationArtifactType::RuntimeSmokeNote, true, true, true },
			{ EJGWorldValidationCategory::PackageSmoke, TEXT("WV_PackageSmoke"), EJGValidationArtifactType::PackageSmokeNote, false, false, true },
			{ EJGWorldValidationCategory::RepoHygiene, TEXT("WV_RepoHygiene"), EJGValidationArtifactType::RepoHygieneNote, false, false, true },
		};
	}

	static TArray<FJGWorldDebugViewSpec> DebugViews()
	{
		return {
			{ EJGWorldDebugView::TerrainHeightBands, TEXT("DBG_TerrainHeightBands"), EJGWorldValidationCategory::TerrainForm, true, false },
			{ EJGWorldDebugView::TerrainSlope, TEXT("DBG_TerrainSlope"), EJGWorldValidationCategory::TerrainForm, true, false },
			{ EJGWorldDebugView::TerrainSlopeBreaks, TEXT("DBG_TerrainSlopeBreaks"), EJGWorldValidationCategory::TerrainForm, true, false },
			{ EJGWorldDebugView::TerrainDrainageBias, TEXT("DBG_TerrainDrainageBias"), EJGWorldValidationCategory::TerrainForm, true, false },
			{ EJGWorldDebugView::TerrainLandformClass, TEXT("DBG_TerrainLandformClass"), EJGWorldValidationCategory::TerrainForm, true, false },
			{ EJGWorldDebugView::LandscapeComponentGrid, TEXT("DBG_LandscapeComponentGrid"), EJGWorldValidationCategory::TerrainContinuity, true, false },
			{ EJGWorldDebugView::ComponentSeamRisk, TEXT("DBG_ComponentSeamRisk"), EJGWorldValidationCategory::TerrainContinuity, true, false },
			{ EJGWorldDebugView::BiomeDominantClass, TEXT("DBG_BiomeDominantClass"), EJGWorldValidationCategory::BiomeMask, true, false },
			{ EJGWorldDebugView::BiomeMaskWeights, TEXT("DBG_BiomeMaskWeights"), EJGWorldValidationCategory::BiomeMask, true, false },
			{ EJGWorldDebugView::BiomeTransitionBands, TEXT("DBG_BiomeTransitionBands"), EJGWorldValidationCategory::BiomeMask, true, false },
			{ EJGWorldDebugView::WetnessMask, TEXT("DBG_WetnessMask"), EJGWorldValidationCategory::BiomeMask, true, false },
			{ EJGWorldDebugView::CanopyDensity, TEXT("DBG_CanopyDensity"), EJGWorldValidationCategory::BiomeMask, true, false },
			{ EJGWorldDebugView::PCGDensity, TEXT("DBG_PCGDensity"), EJGWorldValidationCategory::PCGOutput, true, false },
			{ EJGWorldDebugView::PCGSpawnClass, TEXT("DBG_PCGSpawnClass"), EJGWorldValidationCategory::PCGOutput, true, false },
			{ EJGWorldDebugView::PCGExclusionZones, TEXT("DBG_PCGExclusionZones"), EJGWorldValidationCategory::PCGOutput, true, false },
			{ EJGWorldDebugView::TraversalResistance, TEXT("DBG_TraversalResistance"), EJGWorldValidationCategory::TraversalResistance, true, false },
			{ EJGWorldDebugView::TraversalClass, TEXT("DBG_TraversalClass"), EJGWorldValidationCategory::TraversalResistance, true, false },
			{ EJGWorldDebugView::RouteClass, TEXT("DBG_RouteClass"), EJGWorldValidationCategory::RouteClass, true, false },
			{ EJGWorldDebugView::RouteCostGradient, TEXT("DBG_RouteCostGradient"), EJGWorldValidationCategory::RouteClass, true, false },
			{ EJGWorldDebugView::RouteCandidateGraph, TEXT("DBG_RouteCandidateGraph"), EJGWorldValidationCategory::RouteClass, true, false },
			{ EJGWorldDebugView::FalseAffordanceMarkers, TEXT("DBG_FalseAffordanceMarkers"), EJGWorldValidationCategory::RouteClass, true, false },
			{ EJGWorldDebugView::PlayerStepProbe, TEXT("DBG_PlayerStepProbe"), EJGWorldValidationCategory::TraversalResistance, true, false },
			{ EJGWorldDebugView::NightVisibilityBands, TEXT("DBG_NightVisibilityBands"), EJGWorldValidationCategory::WeatherNightImpact, true, false },
			{ EJGWorldDebugView::RainVisibilityImpact, TEXT("DBG_RainVisibilityImpact"), EJGWorldValidationCategory::WeatherNightImpact, true, false },
			{ EJGWorldDebugView::WetTraversalDelta, TEXT("DBG_WetTraversalDelta"), EJGWorldValidationCategory::WeatherNightImpact, true, false },
			{ EJGWorldDebugView::LandmarkSilhouetteCheck, TEXT("DBG_LandmarkSilhouetteCheck"), EJGWorldValidationCategory::NoMapReadability, true, false },
			{ EJGWorldDebugView::ReadabilityOcclusion, TEXT("DBG_ReadabilityOcclusion"), EJGWorldValidationCategory::NoMapReadability, true, false },
			{ EJGWorldDebugView::WorldPartitionCellState, TEXT("DBG_WorldPartitionCellState"), EJGWorldValidationCategory::WorldPartitionContinuity, true, false },
			{ EJGWorldDebugView::StreamingBoundaryContinuity, TEXT("DBG_StreamingBoundaryContinuity"), EJGWorldValidationCategory::WorldPartitionContinuity, true, false },
			{ EJGWorldDebugView::CellEdgePCGContinuity, TEXT("DBG_CellEdgePCGContinuity"), EJGWorldValidationCategory::WorldPartitionContinuity, true, false },
			{ EJGWorldDebugView::CellEdgeTraversalContinuity, TEXT("DBG_CellEdgeTraversalContinuity"), EJGWorldValidationCategory::WorldPartitionContinuity, true, false },
			{ EJGWorldDebugView::HLODContext, TEXT("DBG_HLODContext"), EJGWorldValidationCategory::WorldPartitionContinuity, true, false },
		};
	}

	static TArray<FJGFirstPersonScreenshotSpec> RequiredFirstPersonScreenshots()
	{
		return {
			{ TEXT("FPV_Coast_ApproachFromJungle_ClearDay_01"), EJGTerrainLandformClass::Coast, EJGValidationCondition::ClearDay, TEXT("Approach") },
			{ TEXT("FPV_Creek_DownstreamDecision_Rain_01"), EJGTerrainLandformClass::Creek, EJGValidationCondition::Rain, TEXT("Decision") },
			{ TEXT("FPV_Ridge_LineReadable_ClearDay_01"), EJGTerrainLandformClass::Ridge, EJGValidationCondition::ClearDay, TEXT("Decision") },
			{ TEXT("FPV_Saddle_PassDecision_ClearDay_01"), EJGTerrainLandformClass::Saddle, EJGValidationCondition::ClearDay, TEXT("Decision") },
			{ TEXT("FPV_Gorge_BlockOrExit_WetPostRain_01"), EJGTerrainLandformClass::Gorge, EJGValidationCondition::WetPostRain, TEXT("Confirmation") },
			{ TEXT("FPV_CanopyGap_FalseCue_ClearDay_01"), EJGTerrainLandformClass::CanopyGap, EJGValidationCondition::ClearDay, TEXT("Reversal") },
			{ TEXT("FPV_SlopeBreak_TraversalChange_Fog_01"), EJGTerrainLandformClass::SlopeBreak, EJGValidationCondition::Fog, TEXT("Confirmation") },
		};
	}

	static TArray<FString> IgnoredLocalArtifactRoots()
	{
		return {
			TEXT("/ValidationArtifacts/"),
			TEXT("/LocalValidationOutput/"),
			TEXT("/PackagedBuilds/"),
			TEXT("/Screenshots/"),
			TEXT("/Videos/"),
			TEXT("/Logs/"),
			TEXT("/Saved/"),
			TEXT("/Intermediate/"),
			TEXT("/DerivedDataCache/"),
		};
	}
};
