#pragma once

#include "CoreMinimal.h"
#include "JungleWorldTerrainBiomeRunway.h"

namespace JungleWorldMaskData
{
	enum class EBiomeMaskId : uint8
	{
		ElevationBand,
		SlopeBand,
		AspectExposure,
		TopographicWetness,
		RidgeInfluence,
		ValleyInfluence,
		CoastInfluence,
		StreamInfluence,
		CanopyClosure,
		UnderstoryDensity,
		LianaTangle,
		RootButtressComplexity,
		FallenTimberLoad,
		DisturbanceGap,
		RegrowthThicket,
		VisibilityOcclusion,
		SoundPropagation,
		SoundMasking,
		SkyWindow,
		FootingRisk,
		EcosystemTellStrength,
		EvidenceRetention,
		EvidenceConfusion,
		NightDangerPressure,
		TraversalResistance,
		TraversalClass
	};

	enum class ETraversalMaskClass : uint8
	{
		Affordance,
		NeutralFriction,
		HazardFriction,
		SoftBlocker,
		HardBlocker,
		FalseAffordance,
		EvidenceSensitive
	};

	enum class EMaskDebugView : uint8
	{
		ElevationBand,
		SlopeBand,
		AspectExposure,
		TopographicWetness,
		RidgeValley,
		CoastInfluence,
		CanopyClosure,
		DisturbanceGap,
		VisibilityOcclusion,
		SoundPropagation,
		SoundMasking,
		SkyWindow,
		FootingRisk,
		TraversalResistance,
		TraversalClass,
		FalseAffordance,
		EvidenceRetention,
		NightDangerPressure,
		ForbiddenUIDrift
	};

	struct FMaskContract
	{
		EBiomeMaskId Id = EBiomeMaskId::ElevationBand;
		FName StableName;
		float MinimumValue = 0.0f;
		float MaximumValue = 1.0f;
		bool bNormalized = true;
		bool bDeveloperOnly = true;
	};

	struct FMaskSample
	{
		FName Id;
		EBiomeMaskId Mask = EBiomeMaskId::ElevationBand;
		JungleWorldTerrainBiomeRunway::ETerrainChannel SourceChannel = JungleWorldTerrainBiomeRunway::ETerrainChannel::Height;
		float Value = 0.0f;
		float Normalized = 0.0f;
	};

	struct FTraversalMaskRule
	{
		ETraversalMaskClass Class = ETraversalMaskClass::NeutralFriction;
		JungleWorldTerrainBiomeRunway::ETraversalCategory TraversalCategory = JungleWorldTerrainBiomeRunway::ETraversalCategory::NeutralFriction;
		float MinimumResistance = 0.0f;
		float MaximumResistance = 1.0f;
		FName StableName;
		bool bAllowsPriorityOverride = false;
		FName PriorityReason;
	};

	struct FBiomeMaskSample
	{
		FName Id;
		FVector WorldPosition = FVector::ZeroVector;
		float ElevationBand = 0.0f;
		float SlopeBand = 0.0f;
		float AspectExposure = 0.0f;
		float TopographicWetness = 0.0f;
		float RidgeInfluence = 0.0f;
		float ValleyInfluence = 0.0f;
		float CoastInfluence = 0.0f;
		float StreamInfluence = 0.0f;
		float CanopyClosure = 0.0f;
		float UnderstoryDensity = 0.0f;
		float LianaTangle = 0.0f;
		float RootButtressComplexity = 0.0f;
		float FallenTimberLoad = 0.0f;
		float DisturbanceGap = 0.0f;
		float RegrowthThicket = 0.0f;
		float VisibilityOcclusion = 0.0f;
		float SoundPropagation = 0.0f;
		float SoundMasking = 0.0f;
		float SkyWindow = 0.0f;
		float FootingRisk = 0.0f;
		float EcosystemTellStrength = 0.0f;
		float EvidenceRetention = 0.0f;
		float EvidenceConfusion = 0.0f;
		float NightDangerPressure = 0.0f;
		float TraversalResistance = 0.0f;
		JungleWorldTerrainBiomeRunway::EBiomeOutput Output = JungleWorldTerrainBiomeRunway::EBiomeOutput::CreekRiparian;
		EBiomeMaskId DominantMask = EBiomeMaskId::TopographicWetness;
		ETraversalMaskClass TraversalClass = ETraversalMaskClass::NeutralFriction;
		float Strength = 0.0f;
	};

	struct FMaskValidationReportSpec
	{
		FName Id;
		EMaskDebugView DebugView = EMaskDebugView::ElevationBand;
		bool bDeveloperOnly = true;
		bool bMustRejectPlayerNavigationUi = true;
	};

	TConstArrayView<FMaskContract> GetBiomeMaskContracts();
	TConstArrayView<FTraversalMaskRule> GetTraversalMaskRules();
	TConstArrayView<FMaskValidationReportSpec> GetMaskValidationReportSpecs();

	bool IsNormalizedMaskValue(float Value);
	float ClampMaskValue(float Value);
	float CalculateTraversalResistance(const FBiomeMaskSample& Sample);
	bool IsFalseAffordanceCandidate(const FBiomeMaskSample& Sample);
	ETraversalMaskClass ClassifyTraversalMask(const FBiomeMaskSample& Sample);

	const TCHAR* LexToString(EBiomeMaskId Mask);
	const TCHAR* LexToString(ETraversalMaskClass Class);
	const TCHAR* LexToString(EMaskDebugView View);
}
