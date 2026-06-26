#include "JungleWorldMaskData.h"

namespace JungleWorldMaskData
{
	namespace
	{
		const FMaskContract BiomeMaskContracts[] =
		{
			{ EBiomeMaskId::ElevationBand, TEXT("M_ElevationBand"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::SlopeBand, TEXT("M_SlopeBand"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::AspectExposure, TEXT("M_AspectExposure"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::TopographicWetness, TEXT("M_TopographicWetness"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::RidgeInfluence, TEXT("M_RidgeInfluence"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::ValleyInfluence, TEXT("M_ValleyInfluence"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::CoastInfluence, TEXT("M_CoastInfluence"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::StreamInfluence, TEXT("M_StreamInfluence"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::CanopyClosure, TEXT("M_CanopyClosure"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::UnderstoryDensity, TEXT("M_UnderstoryDensity"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::LianaTangle, TEXT("M_LianaTangle"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::RootButtressComplexity, TEXT("M_RootButtressComplexity"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::FallenTimberLoad, TEXT("M_FallenTimberLoad"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::DisturbanceGap, TEXT("M_DisturbanceGap"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::RegrowthThicket, TEXT("M_RegrowthThicket"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::VisibilityOcclusion, TEXT("M_VisibilityOcclusion"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::SoundPropagation, TEXT("M_SoundPropagation"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::SoundMasking, TEXT("M_SoundMasking"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::SkyWindow, TEXT("M_SkyWindow"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::FootingRisk, TEXT("M_FootingRisk"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::EcosystemTellStrength, TEXT("M_EcosystemTellStrength"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::EvidenceRetention, TEXT("M_EvidenceRetention"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::EvidenceConfusion, TEXT("M_EvidenceConfusion"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::NightDangerPressure, TEXT("M_NightDangerPressure"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::TraversalResistance, TEXT("TR_Resistance"), 0.0f, 1.0f, true, true },
			{ EBiomeMaskId::TraversalClass, TEXT("TR_Class"), 0.0f, static_cast<float>(ETraversalMaskClass::EvidenceSensitive), false, true }
		};

		const FTraversalMaskRule TraversalMaskRules[] =
		{
			{ ETraversalMaskClass::Affordance, JungleWorldTerrainBiomeRunway::ETraversalCategory::Affordance, 0.00f, 0.25f, TEXT("TR_Affordance"), false, NAME_None },
			{ ETraversalMaskClass::NeutralFriction, JungleWorldTerrainBiomeRunway::ETraversalCategory::NeutralFriction, 0.20f, 0.45f, TEXT("TR_NeutralFriction"), false, NAME_None },
			{ ETraversalMaskClass::HazardFriction, JungleWorldTerrainBiomeRunway::ETraversalCategory::SoftBlocker, 0.35f, 0.65f, TEXT("TR_HazardFriction"), false, NAME_None },
			{ ETraversalMaskClass::SoftBlocker, JungleWorldTerrainBiomeRunway::ETraversalCategory::SoftBlocker, 0.55f, 0.80f, TEXT("TR_SoftBlocker"), false, NAME_None },
			{ ETraversalMaskClass::HardBlocker, JungleWorldTerrainBiomeRunway::ETraversalCategory::HardBlocker, 0.85f, 1.00f, TEXT("TR_HardBlocker"), true, TEXT("Direct physical blocker masks can override composed resistance") },
			{ ETraversalMaskClass::FalseAffordance, JungleWorldTerrainBiomeRunway::ETraversalCategory::FalseAffordance, 0.60f, 0.90f, TEXT("TR_FalseAffordance"), false, NAME_None },
			{ ETraversalMaskClass::EvidenceSensitive, JungleWorldTerrainBiomeRunway::ETraversalCategory::NeutralFriction, 0.20f, 0.70f, TEXT("TR_EvidenceSensitive"), true, TEXT("Evidence masks can override ordinary friction labels inside this band") }
		};

		const FMaskValidationReportSpec MaskValidationReportSpecs[] =
		{
			{ TEXT("JungleMaskCoverageReport"), EMaskDebugView::ElevationBand, true, true },
			{ TEXT("JungleTraversalClassReport"), EMaskDebugView::TraversalClass, true, true },
			{ TEXT("JungleFalseAffordanceReport"), EMaskDebugView::FalseAffordance, true, true },
			{ TEXT("JungleHardBlockerContinuityReport"), EMaskDebugView::TraversalResistance, true, true },
			{ TEXT("JungleSkyWindowDistributionReport"), EMaskDebugView::SkyWindow, true, true },
			{ TEXT("JungleSoundMaskingZoneReport"), EMaskDebugView::SoundMasking, true, true },
			{ TEXT("JungleEvidenceRetentionReport"), EMaskDebugView::EvidenceRetention, true, true },
			{ TEXT("JungleForbiddenUIDriftReport"), EMaskDebugView::ForbiddenUIDrift, true, true }
		};
	}

	TConstArrayView<FMaskContract> GetBiomeMaskContracts()
	{
		return BiomeMaskContracts;
	}

	TConstArrayView<FTraversalMaskRule> GetTraversalMaskRules()
	{
		return TraversalMaskRules;
	}

	TConstArrayView<FMaskValidationReportSpec> GetMaskValidationReportSpecs()
	{
		return MaskValidationReportSpecs;
	}

	bool IsNormalizedMaskValue(float Value)
	{
		return Value >= 0.0f && Value <= 1.0f;
	}

	float ClampMaskValue(float Value)
	{
		return FMath::Clamp(Value, 0.0f, 1.0f);
	}

	float CalculateTraversalResistance(const FBiomeMaskSample& Sample)
	{
		float Resistance =
			0.20f * ClampMaskValue(Sample.SlopeBand) +
			0.16f * ClampMaskValue(Sample.UnderstoryDensity) +
			0.12f * ClampMaskValue(Sample.LianaTangle) +
			0.12f * ClampMaskValue(Sample.FootingRisk) +
			0.10f * ClampMaskValue(Sample.TopographicWetness) +
			0.08f * ClampMaskValue(Sample.FallenTimberLoad) +
			0.08f * ClampMaskValue(Sample.RootButtressComplexity) +
			0.06f * ClampMaskValue(Sample.VisibilityOcclusion) +
			0.04f * ClampMaskValue(Sample.RegrowthThicket) +
			0.04f * ClampMaskValue(Sample.NightDangerPressure);

		if (Sample.RidgeInfluence > 0.70f && Sample.SlopeBand < 0.45f)
		{
			Resistance -= 0.10f;
		}

		if (Sample.StreamInfluence > 0.65f && Sample.TopographicWetness > 0.65f)
		{
			Resistance += 0.12f;
		}

		return ClampMaskValue(Resistance);
	}

	bool IsFalseAffordanceCandidate(const FBiomeMaskSample& Sample)
	{
		const bool bHasTemptationCue =
			Sample.SkyWindow >= 0.55f ||
			Sample.VisibilityOcclusion <= 0.35f ||
			Sample.DisturbanceGap >= 0.60f;

		const bool bHasReadableHiddenCost =
			Sample.LianaTangle >= 0.55f ||
			Sample.RegrowthThicket >= 0.55f ||
			Sample.FootingRisk >= 0.65f ||
			Sample.FallenTimberLoad >= 0.60f;

		return bHasTemptationCue && bHasReadableHiddenCost;
	}

	ETraversalMaskClass ClassifyTraversalMask(const FBiomeMaskSample& Sample)
	{
		if (Sample.SlopeBand >= 0.88f || Sample.FootingRisk >= 0.90f || Sample.FallenTimberLoad >= 0.85f)
		{
			return ETraversalMaskClass::HardBlocker;
		}

		if (Sample.TopographicWetness >= 0.90f && Sample.RootButtressComplexity >= 0.60f)
		{
			return ETraversalMaskClass::HardBlocker;
		}

		const float Resistance = CalculateTraversalResistance(Sample);

		if (Resistance >= 0.85f)
		{
			return ETraversalMaskClass::HardBlocker;
		}

		if (IsFalseAffordanceCandidate(Sample) && Resistance >= 0.60f && Resistance <= 0.90f)
		{
			return ETraversalMaskClass::FalseAffordance;
		}

		if ((Sample.EvidenceRetention >= 0.65f || Sample.EvidenceConfusion >= 0.65f) && Resistance >= 0.20f && Resistance <= 0.70f)
		{
			return ETraversalMaskClass::EvidenceSensitive;
		}

		if (Resistance >= 0.65f)
		{
			return ETraversalMaskClass::SoftBlocker;
		}

		if (Resistance >= 0.45f)
		{
			return ETraversalMaskClass::HazardFriction;
		}

		if (Resistance <= 0.25f)
		{
			return ETraversalMaskClass::Affordance;
		}

		return ETraversalMaskClass::NeutralFriction;
	}

	const TCHAR* LexToString(EBiomeMaskId Mask)
	{
		switch (Mask)
		{
		case EBiomeMaskId::ElevationBand: return TEXT("M_ElevationBand");
		case EBiomeMaskId::SlopeBand: return TEXT("M_SlopeBand");
		case EBiomeMaskId::AspectExposure: return TEXT("M_AspectExposure");
		case EBiomeMaskId::TopographicWetness: return TEXT("M_TopographicWetness");
		case EBiomeMaskId::RidgeInfluence: return TEXT("M_RidgeInfluence");
		case EBiomeMaskId::ValleyInfluence: return TEXT("M_ValleyInfluence");
		case EBiomeMaskId::CoastInfluence: return TEXT("M_CoastInfluence");
		case EBiomeMaskId::StreamInfluence: return TEXT("M_StreamInfluence");
		case EBiomeMaskId::CanopyClosure: return TEXT("M_CanopyClosure");
		case EBiomeMaskId::UnderstoryDensity: return TEXT("M_UnderstoryDensity");
		case EBiomeMaskId::LianaTangle: return TEXT("M_LianaTangle");
		case EBiomeMaskId::RootButtressComplexity: return TEXT("M_RootButtressComplexity");
		case EBiomeMaskId::FallenTimberLoad: return TEXT("M_FallenTimberLoad");
		case EBiomeMaskId::DisturbanceGap: return TEXT("M_DisturbanceGap");
		case EBiomeMaskId::RegrowthThicket: return TEXT("M_RegrowthThicket");
		case EBiomeMaskId::VisibilityOcclusion: return TEXT("M_VisibilityOcclusion");
		case EBiomeMaskId::SoundPropagation: return TEXT("M_SoundPropagation");
		case EBiomeMaskId::SoundMasking: return TEXT("M_SoundMasking");
		case EBiomeMaskId::SkyWindow: return TEXT("M_SkyWindow");
		case EBiomeMaskId::FootingRisk: return TEXT("M_FootingRisk");
		case EBiomeMaskId::EcosystemTellStrength: return TEXT("M_EcosystemTellStrength");
		case EBiomeMaskId::EvidenceRetention: return TEXT("M_EvidenceRetention");
		case EBiomeMaskId::EvidenceConfusion: return TEXT("M_EvidenceConfusion");
		case EBiomeMaskId::NightDangerPressure: return TEXT("M_NightDangerPressure");
		case EBiomeMaskId::TraversalResistance: return TEXT("TR_Resistance");
		case EBiomeMaskId::TraversalClass: return TEXT("TR_Class");
		default: return TEXT("Unknown");
		}
	}

	const TCHAR* LexToString(ETraversalMaskClass Class)
	{
		switch (Class)
		{
		case ETraversalMaskClass::Affordance: return TEXT("TR_Affordance");
		case ETraversalMaskClass::NeutralFriction: return TEXT("TR_NeutralFriction");
		case ETraversalMaskClass::HazardFriction: return TEXT("TR_HazardFriction");
		case ETraversalMaskClass::SoftBlocker: return TEXT("TR_SoftBlocker");
		case ETraversalMaskClass::HardBlocker: return TEXT("TR_HardBlocker");
		case ETraversalMaskClass::FalseAffordance: return TEXT("TR_FalseAffordance");
		case ETraversalMaskClass::EvidenceSensitive: return TEXT("TR_EvidenceSensitive");
		default: return TEXT("Unknown");
		}
	}

	const TCHAR* LexToString(EMaskDebugView View)
	{
		switch (View)
		{
		case EMaskDebugView::ElevationBand: return TEXT("DBG_Mask_ElevationBand");
		case EMaskDebugView::SlopeBand: return TEXT("DBG_Mask_SlopeBand");
		case EMaskDebugView::AspectExposure: return TEXT("DBG_Mask_AspectExposure");
		case EMaskDebugView::TopographicWetness: return TEXT("DBG_Mask_TopographicWetness");
		case EMaskDebugView::RidgeValley: return TEXT("DBG_Mask_RidgeValley");
		case EMaskDebugView::CoastInfluence: return TEXT("DBG_Mask_CoastInfluence");
		case EMaskDebugView::CanopyClosure: return TEXT("DBG_Mask_CanopyClosure");
		case EMaskDebugView::DisturbanceGap: return TEXT("DBG_Mask_DisturbanceGap");
		case EMaskDebugView::VisibilityOcclusion: return TEXT("DBG_Mask_VisibilityOcclusion");
		case EMaskDebugView::SoundPropagation: return TEXT("DBG_Mask_SoundPropagation");
		case EMaskDebugView::SoundMasking: return TEXT("DBG_Mask_SoundMasking");
		case EMaskDebugView::SkyWindow: return TEXT("DBG_Mask_SkyWindow");
		case EMaskDebugView::FootingRisk: return TEXT("DBG_Mask_FootingRisk");
		case EMaskDebugView::TraversalResistance: return TEXT("DBG_Mask_TraversalResistance");
		case EMaskDebugView::TraversalClass: return TEXT("DBG_Mask_TraversalClass");
		case EMaskDebugView::FalseAffordance: return TEXT("DBG_Mask_FalseAffordance");
		case EMaskDebugView::EvidenceRetention: return TEXT("DBG_Mask_EvidenceRetention");
		case EMaskDebugView::NightDangerPressure: return TEXT("DBG_Mask_NightDangerPressure");
		case EMaskDebugView::ForbiddenUIDrift: return TEXT("DBG_Mask_ForbiddenUIDrift");
		default: return TEXT("Unknown");
		}
	}
}
