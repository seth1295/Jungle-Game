#pragma once

#include "CoreMinimal.h"
#include "JungleEcosystemMaskSpec.h"
#include "JungleMacroTerrainSpec.h"
#include "JunglePCGBiomeSpec.h"

/** Creek and wet-valley subzones consumed by later PCG graph assets. */
enum class EJungleCreekRiparianWetValleyZone : uint8
{
	OutsideCreekInfluence,
	CreekChannelHardWater,
	CreekBankAffordance,
	CreekCrossingCandidate,
	RiparianBench,
	WetValleyFloor,
	MuddyDeadfallSoftBlocker,
	WaitAWhileSoftBlocker,
	WaterSoundAnchor,
	FalseCreekAffordance,
};

/** Placeholder creek evidence types; these are contracts, not final water, audio, or wildlife systems. */
enum class EJungleCreekEvidenceCue : uint8
{
	ChannelSound,
	BankVegetationChange,
	WetRockRoot,
	MudShelf,
	DeadfallTangle,
	CrossingShallowShelf,
	LeechTickRiskNote,
	FrogInsectSoundNote,
	MuffledRainWaterNote,
	FalseBankOpening,
};

enum class EJungleCreekReachClass : uint8
{
	HeadwaterSeep,
	EphemeralGully,
	NarrowCreek,
	WetValley,
	Cascade,
	Waterfall,
	LowlandCreek,
	CreekMouth,
	TidalReach,
};

enum class EJungleCreekBankAffordance : uint8
{
	OpenTravel,
	SlowTravel,
	CarefulFooting,
	SoftBlocker,
	HardBlocker,
	FalseAffordance,
	RouteTrap,
	CrossingApproach,
	ForcedDetour,
};

enum class EJungleCreekCrossingValidity : uint8
{
	ValidPrimary,
	ValidRisky,
	FalseCrossing,
	Blocked,
	WeatherChanged,
	NeedsReview,
};

enum class EJungleCreekCrossingType : uint8
{
	DryRockHop,
	ShallowWade,
	RootStep,
	FallenLog,
	GravelBar,
	BoulderChoke,
	NarrowPinch,
	PoolEdgeSkirt,
	UnsafeFlooded,
	FalseCrossing,
};

enum class EJungleWaterSoundAnchorType : uint8
{
	Trickle,
	Riffle,
	Run,
	Rapid,
	Cascade,
	Waterfall,
	DeepPool,
	DebrisJam,
	Confluence,
	CreekMouth,
	FrogWetBand,
	InsectWetBand,
	RainDrip,
	ValleyMuffle,
};

enum class EJungleCreekAcousticMaskState : uint8
{
	ClearDirectional,
	VegetationMuffled,
	ValleyReverb,
	RainMasked,
	WaterfallDominant,
	ConfluenceConfused,
	NightBiophonyDense,
};

enum class EJungleRiparianRainState : uint8
{
	DryBaseline,
	LightRain,
	HeavyRain,
	StormBurst,
	PostRainRising,
	PostRainFalling,
};

enum class EJungleCreekFlowSpeedClass : uint8
{
	Still,
	Slow,
	Moderate,
	Fast,
	FloodPulse,
};

enum class EJungleCreekBedSurface : uint8
{
	SandMud,
	Gravel,
	Cobble,
	Boulder,
	Bedrock,
	RootMat,
	Debris,
};

enum class EJungleCreekBankShape : uint8
{
	LowShelf,
	SlopedMud,
	RootStep,
	RootedToe,
	Undercut,
	CutBank,
	BoulderStep,
	VegetationGap,
	SwampMargin,
};

enum class EJungleCreekVisibilityClass : uint8
{
	Clear,
	PartiallyOccluded,
	Glare,
	FoamObscured,
	RainObscured,
	NightObscured,
};

enum class EJungleCreekRiskLevel : uint8
{
	Low,
	Medium,
	High,
	Unsafe,
};

enum class EJungleCreekVegetationRisk : uint8
{
	None,
	Snag,
	DenseExit,
	WaitAWhile,
	Deadfall,
};

enum class EJungleCreekRouteEvidence : uint8
{
	WaterDirection,
	ShallowRiffle,
	ExposedStone,
	LowOppositeBank,
	DeepPoolDarkWater,
	UndercutExitBank,
	RainSwollen,
	SlipperyRockSheen,
	DenseExitVegetation,
	DebrisJamBlock,
};

enum class EJungleCreekDebugView : uint8
{
	FlowDirection,
	ReachClass,
	BankAffordance,
	Wetness,
	CrossingCandidates,
	CrossingValidity,
	RouteEvidence,
	RouteTraps,
	SoundAnchors,
	RainResponse,
};

/** Data-asset-shaped source contract for later reflected creek reach assets. */
struct UCreekReachDataAsset
{
	FName ReachId;
	int32 CatchmentId = INDEX_NONE;
	int32 DrainageOrder = 0;
	FVector FlowDirectionVector = FVector::ForwardVector;
	FName DownstreamNodeId;
	TArray<FName> UpstreamNodeIds;
	FName NearestOutletId;
	EJungleCreekReachClass ReachClass = EJungleCreekReachClass::NarrowCreek;
	float ReachSlope = 0.0f;
	float ValleyConfinement = 0.0f;
	float BankAsymmetry = 0.0f;
	EJungleRiparianRainState RecentRainResponse = EJungleRiparianRainState::DryBaseline;
	bool bHasTidalInfluence = false;
	float CrossingDensityBudget = 0.0f;
};

struct FCreekReachRuntimeState
{
	FName ReachId;
	EJungleRiparianRainState RainState = EJungleRiparianRainState::DryBaseline;
	EJungleCreekFlowSpeedClass FlowSpeedClass = EJungleCreekFlowSpeedClass::Slow;
	float NormalizedWetness = 0.0f;
	float NormalizedAudioMasking = 0.0f;
	bool bEphemeralGullyActive = false;
	bool bCrossingsMayHaveChanged = false;
};

struct FCreekCrossingCandidate
{
	FName CandidateId;
	FName CreekReachId;
	int32 CatchmentId = INDEX_NONE;
	FVector WorldLocation = FVector::ZeroVector;
	FName EntryBankSide;
	FName ExitBankSide;
	FVector ApproachVector = FVector::ForwardVector;
	FVector ExitVector = FVector::ForwardVector;
	FVector FlowDirectionVector = FVector::ForwardVector;
	float ChannelWidthM = 0.0f;
	float WaterDepthEstimateM = 0.0f;
	EJungleCreekFlowSpeedClass FlowSpeedClass = EJungleCreekFlowSpeedClass::Slow;
	EJungleCreekBedSurface BedSurface = EJungleCreekBedSurface::Gravel;
	EJungleCreekBankShape BankEntryShape = EJungleCreekBankShape::LowShelf;
	EJungleCreekBankShape BankExitShape = EJungleCreekBankShape::LowShelf;
	EJungleCreekVisibilityClass VisibilityClass = EJungleCreekVisibilityClass::Clear;
	EJungleCreekRiskLevel FootingRisk = EJungleCreekRiskLevel::Low;
	EJungleCreekRiskLevel WaterRisk = EJungleCreekRiskLevel::Low;
	EJungleCreekVegetationRisk VegetationRisk = EJungleCreekVegetationRisk::None;
	EJungleRiparianRainState RecentRainState = EJungleRiparianRainState::DryBaseline;
	TArray<EJungleCreekRouteEvidence> PlayerReadableEvidence;
	EJungleCreekCrossingValidity ValidationStatus = EJungleCreekCrossingValidity::NeedsReview;
	EJungleCreekCrossingType CrossingType = EJungleCreekCrossingType::ShallowWade;
	FName DebugReason;

	bool HasApproachChannelAndExit() const
	{
		return !EntryBankSide.IsNone() && !ExitBankSide.IsNone() && FlowDirectionVector.SizeSquared() > UE_SMALL_NUMBER;
	}
};

struct FRiparianBiomeOutput
{
	FName OutputId;
	EJungleCreekRiparianWetValleyZone Zone = EJungleCreekRiparianWetValleyZone::RiparianBench;
	EJunglePCGSpawnRuleKind RuleKind = EJunglePCGSpawnRuleKind::GroundClutterPlaceholder;
	EJunglePCGTerrainFrictionClass FrictionClass = EJunglePCGTerrainFrictionClass::ReadablePlantFriction;
	TArray<FName> RequiredMasks;
	bool bDeveloperOnly = true;
};

struct FWaterSoundAnchor
{
	FName AnchorId;
	FName CreekReachId;
	EJungleWaterSoundAnchorType AnchorType = EJungleWaterSoundAnchorType::Run;
	EJungleCreekAcousticMaskState AcousticMaskState = EJungleCreekAcousticMaskState::ClearDirectional;
	float RadiusMeters = 24.0f;
	float NormalizedMasking = 0.0f;
	bool bDeveloperOnly = true;
};

struct FRouteEvidencePoint
{
	FName EvidenceId;
	FName CreekReachId;
	FVector WorldLocation = FVector::ZeroVector;
	EJungleCreekRouteEvidence Evidence = EJungleCreekRouteEvidence::WaterDirection;
	float EvidenceStrength = 1.0f;
	bool bDeveloperOnly = true;
};

struct FWetValleyRiskZone
{
	FName RiskZoneId;
	FName CreekReachId;
	EJungleCreekBankAffordance Affordance = EJungleCreekBankAffordance::SlowTravel;
	EJungleCreekRiskLevel FootingRisk = EJungleCreekRiskLevel::Medium;
	EJungleCreekVegetationRisk VegetationRisk = EJungleCreekVegetationRisk::Snag;
	float NormalizedWetness = 0.0f;
	float NormalizedVisibilityLoss = 0.0f;
};

/** Reviewable contract for a future creek/riparian PCG rule. */
struct FJungleCreekRiparianRuleContract
{
	EJungleCreekRiparianWetValleyZone Zone = EJungleCreekRiparianWetValleyZone::OutsideCreekInfluence;
	EJungleCreekEvidenceCue EvidenceCue = EJungleCreekEvidenceCue::BankVegetationChange;
	EJunglePCGSpawnRuleKind RuleKind = EJunglePCGSpawnRuleKind::GroundClutterPlaceholder;
	EJunglePCGTerrainFrictionClass FrictionClass = EJunglePCGTerrainFrictionClass::Open;
	TArray<FString> RequiredMaskNames;
	float MaxInstancesPerHectare = 0.0f;
	float MinimumSpacingMeters = 1.0f;
	bool bCanMarkCrossingCandidate = false;
	bool bCanCreateFalseAffordance = false;
	bool bDeveloperOnly = true;

	bool IsReviewable() const
	{
		return MaxInstancesPerHectare >= 0.0f
			&& MinimumSpacingMeters > 0.0f
			&& RequiredMaskNames.Num() > 0;
	}
};

struct FJungleCreekRiparianWetValleyEcosystemSpec
{
	static constexpr int32 EcosystemPassVersion = 2;
	static constexpr float ChannelEdgeMeters = 8.0f;
	static constexpr float CreekBankAffordanceMeters = 24.0f;
	static constexpr float RiparianBenchMeters = 48.0f;
	static constexpr float WetValleyInfluenceMeters = FJunglePCGBiomeFrameworkSpec::CreekInfluenceMeters;
	static constexpr float MaxCrossingSlopeDegrees = 18.0f;
	static constexpr float MaxWetValleyFloorSlopeDegrees = 12.0f;
	static constexpr float MinimumCrossingAffordance = 0.72f;
	static constexpr float MaximumCrossingFootingRisk = 0.48f;
	static constexpr float MuddySoftBlockerWetness = 0.74f;
	static constexpr float WaitAWhileSoftBlockerCanopy = 0.68f;
	static constexpr float MinimumSoundAnchorPropagation = 0.62f;

	static bool AcceptsFinalWaterSimulation()
	{
		return false;
	}

	static bool AcceptsFinalAudioMix()
	{
		return false;
	}

	static bool AcceptsFullWildlifeAI()
	{
		return false;
	}

	static bool IsInsideCreekInfluence(const FJunglePCGBiomeInputSample& Sample)
	{
		return Sample.CreekDistanceMeters <= WetValleyInfluenceMeters
			|| Sample.GetMaskValue(TEXT("wetness")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold;
	}

	static bool IsPotentialCrossingCandidate(const FJunglePCGBiomeInputSample& Sample)
	{
		return Sample.CreekDistanceMeters <= CreekBankAffordanceMeters
			&& Sample.SlopeDegrees <= MaxCrossingSlopeDegrees
			&& Sample.GetMaskValue(TEXT("affordance_zone")) >= MinimumCrossingAffordance
			&& Sample.GetMaskValue(TEXT("footing_risk")) <= MaximumCrossingFootingRisk
			&& Sample.GetMaskValue(TEXT("hard_blocker")) < FJungleEcosystemMaskSpec::HardBlockerThreshold;
	}

	static EJungleCreekBankAffordance ClassifyBankAffordance(float BankSlopeDegrees, float NormalizedWetness, float VegetationFriction, EJungleCreekBankShape BankShape)
	{
		if (BankShape == EJungleCreekBankShape::CutBank || BankShape == EJungleCreekBankShape::Undercut || BankSlopeDegrees > 38.0f)
		{
			return EJungleCreekBankAffordance::HardBlocker;
		}

		if ((BankShape == EJungleCreekBankShape::SlopedMud || BankShape == EJungleCreekBankShape::SwampMargin) && NormalizedWetness >= 0.75f)
		{
			return EJungleCreekBankAffordance::CarefulFooting;
		}

		if (VegetationFriction >= 0.80f)
		{
			return EJungleCreekBankAffordance::SoftBlocker;
		}

		if (VegetationFriction >= 0.60f && (BankShape == EJungleCreekBankShape::VegetationGap || BankShape == EJungleCreekBankShape::RootedToe))
		{
			return EJungleCreekBankAffordance::FalseAffordance;
		}

		if (BankSlopeDegrees <= 12.0f && NormalizedWetness <= 0.45f && VegetationFriction <= 0.35f)
		{
			return EJungleCreekBankAffordance::OpenTravel;
		}

		return EJungleCreekBankAffordance::SlowTravel;
	}

	static EJungleCreekCrossingValidity ResolveCrossingValidity(const FCreekCrossingCandidate& Candidate)
	{
		if (!Candidate.HasApproachChannelAndExit())
		{
			return EJungleCreekCrossingValidity::NeedsReview;
		}

		if (Candidate.RecentRainState == EJungleRiparianRainState::StormBurst || Candidate.FlowSpeedClass == EJungleCreekFlowSpeedClass::FloodPulse)
		{
			return EJungleCreekCrossingValidity::WeatherChanged;
		}

		if (Candidate.WaterRisk == EJungleCreekRiskLevel::Unsafe || Candidate.FootingRisk == EJungleCreekRiskLevel::Unsafe)
		{
			return EJungleCreekCrossingValidity::Blocked;
		}

		if (Candidate.BankExitShape == EJungleCreekBankShape::Undercut || Candidate.VegetationRisk == EJungleCreekVegetationRisk::DenseExit)
		{
			return EJungleCreekCrossingValidity::FalseCrossing;
		}

		if (Candidate.WaterRisk == EJungleCreekRiskLevel::High || Candidate.FootingRisk == EJungleCreekRiskLevel::High)
		{
			return EJungleCreekCrossingValidity::ValidRisky;
		}

		return EJungleCreekCrossingValidity::ValidPrimary;
	}

	static EJungleCreekRiparianWetValleyZone ClassifyCreekZone(const FJunglePCGBiomeInputSample& Sample)
	{
		if (!IsInsideCreekInfluence(Sample))
		{
			return EJungleCreekRiparianWetValleyZone::OutsideCreekInfluence;
		}

		if (Sample.GetMaskValue(TEXT("false_affordance")) >= FJungleEcosystemMaskSpec::FalseAffordanceThreshold)
		{
			return EJungleCreekRiparianWetValleyZone::FalseCreekAffordance;
		}

		if (Sample.GetMaskValue(TEXT("hard_blocker")) >= FJungleEcosystemMaskSpec::HardBlockerThreshold
			&& Sample.CreekDistanceMeters <= ChannelEdgeMeters)
		{
			return EJungleCreekRiparianWetValleyZone::CreekChannelHardWater;
		}

		if (IsPotentialCrossingCandidate(Sample))
		{
			return EJungleCreekRiparianWetValleyZone::CreekCrossingCandidate;
		}

		if (Sample.GetMaskValue(TEXT("soft_blocker")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold
			&& Sample.GetMaskValue(TEXT("canopy")) >= WaitAWhileSoftBlockerCanopy)
		{
			return EJungleCreekRiparianWetValleyZone::WaitAWhileSoftBlocker;
		}

		if (Sample.GetMaskValue(TEXT("wetness")) >= MuddySoftBlockerWetness
			|| Sample.GetMaskValue(TEXT("footing_risk")) >= FJungleEcosystemMaskSpec::SoftBlockerThreshold)
		{
			return EJungleCreekRiparianWetValleyZone::MuddyDeadfallSoftBlocker;
		}

		if (Sample.GetMaskValue(TEXT("sound_propagation")) >= MinimumSoundAnchorPropagation
			&& Sample.CreekDistanceMeters <= RiparianBenchMeters)
		{
			return EJungleCreekRiparianWetValleyZone::WaterSoundAnchor;
		}

		if (Sample.SlopeDegrees <= MaxWetValleyFloorSlopeDegrees
			&& Sample.GetMaskValue(TEXT("wetness")) >= FJungleEcosystemMaskSpec::AffordanceThreshold)
		{
			return EJungleCreekRiparianWetValleyZone::WetValleyFloor;
		}

		if (Sample.CreekDistanceMeters <= CreekBankAffordanceMeters)
		{
			return EJungleCreekRiparianWetValleyZone::CreekBankAffordance;
		}

		return EJungleCreekRiparianWetValleyZone::RiparianBench;
	}

	static EJunglePCGTerrainFrictionClass ResolveCreekFriction(const FJunglePCGBiomeInputSample& Sample)
	{
		return FJunglePCGBiomeFrameworkSpec::ResolveTerrainFriction(
			Sample.GetMaskValue(TEXT("hard_blocker")),
			Sample.GetMaskValue(TEXT("soft_blocker")),
			Sample.GetMaskValue(TEXT("affordance_zone")),
			Sample.GetMaskValue(TEXT("false_affordance")));
	}

	static TArray<FString> RequiredMaskNames()
	{
		return {
			TEXT("creek_distance"),
			TEXT("wetness"),
			TEXT("slope"),
			TEXT("canopy"),
			TEXT("footing_risk"),
			TEXT("sound_propagation"),
			TEXT("visibility"),
			TEXT("hard_blocker"),
			TEXT("soft_blocker"),
			TEXT("affordance_zone"),
			TEXT("false_affordance"),
			TEXT("M_Creek_ChannelCore"),
			TEXT("M_Creek_WettedBed"),
			TEXT("M_Creek_BankToe"),
			TEXT("M_Creek_UpperBankBench"),
			TEXT("M_Creek_RiparianDense"),
			TEXT("M_Creek_WetValleyFloor"),
			TEXT("M_Creek_MudPatch"),
			TEXT("M_Creek_DebrisJam"),
			TEXT("M_Creek_CrossingCandidate"),
			TEXT("M_Creek_HardBlocker"),
			TEXT("M_Creek_RouteTrap"),
		};
	}

	static TArray<FString> RouteEvidenceNames()
	{
		return {
			TEXT("RouteEvidence_WaterDirection"),
			TEXT("RouteEvidence_ShallowRiffle"),
			TEXT("RouteEvidence_ExposedStone"),
			TEXT("RouteEvidence_LowOppositeBank"),
			TEXT("RouteEvidence_DeepPoolDarkWater"),
			TEXT("RouteEvidence_UndercutExitBank"),
			TEXT("RouteEvidence_RainSwollen"),
			TEXT("RouteEvidence_SlipperyRockSheen"),
			TEXT("RouteEvidence_DenseExitVegetation"),
			TEXT("RouteEvidence_DebrisJamBlock"),
		};
	}

	static TArray<FString> SoundAnchorNames()
	{
		return {
			TEXT("SA_Creek_Trickle"),
			TEXT("SA_Creek_Riffle"),
			TEXT("SA_Creek_Run"),
			TEXT("SA_Creek_Cascade"),
			TEXT("SA_Creek_Waterfall"),
			TEXT("SA_Creek_Confluence"),
			TEXT("SA_Riparian_FrogBand"),
			TEXT("SA_Riparian_InsectWetEdge"),
			TEXT("SA_Rain_CanopyDrip"),
			TEXT("SA_Valley_MuffledDistantWater"),
		};
	}

	static TArray<FString> DebugViewNames()
	{
		return {
			TEXT("DBG_Creek_FlowDirection"),
			TEXT("DBG_Creek_ReachClass"),
			TEXT("DBG_Creek_BankAffordance"),
			TEXT("DBG_Creek_Wetness"),
			TEXT("DBG_Creek_CrossingCandidates"),
			TEXT("DBG_Creek_CrossingValidity"),
			TEXT("DBG_Creek_RouteEvidence"),
			TEXT("DBG_Creek_RouteTraps"),
			TEXT("DBG_Creek_SoundAnchors"),
			TEXT("DBG_Creek_RainResponse"),
		};
	}

	static TArray<FJungleCreekRiparianRuleContract> PlaceholderRuleContracts()
	{
		TArray<FJungleCreekRiparianRuleContract> Rules;

		FJungleCreekRiparianRuleContract BankAffordance;
		BankAffordance.Zone = EJungleCreekRiparianWetValleyZone::CreekBankAffordance;
		BankAffordance.EvidenceCue = EJungleCreekEvidenceCue::BankVegetationChange;
		BankAffordance.RuleKind = EJunglePCGSpawnRuleKind::AffordanceCuePlaceholder;
		BankAffordance.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		BankAffordance.RequiredMaskNames = { TEXT("creek_distance"), TEXT("affordance_zone"), TEXT("visibility") };
		BankAffordance.MaxInstancesPerHectare = 36.0f;
		BankAffordance.MinimumSpacingMeters = 6.0f;
		Rules.Add(BankAffordance);

		FJungleCreekRiparianRuleContract CrossingCandidate;
		CrossingCandidate.Zone = EJungleCreekRiparianWetValleyZone::CreekCrossingCandidate;
		CrossingCandidate.EvidenceCue = EJungleCreekEvidenceCue::CrossingShallowShelf;
		CrossingCandidate.RuleKind = EJunglePCGSpawnRuleKind::AffordanceCuePlaceholder;
		CrossingCandidate.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		CrossingCandidate.RequiredMaskNames = { TEXT("creek_distance"), TEXT("slope"), TEXT("footing_risk"), TEXT("affordance_zone") };
		CrossingCandidate.MaxInstancesPerHectare = 8.0f;
		CrossingCandidate.MinimumSpacingMeters = 18.0f;
		CrossingCandidate.bCanMarkCrossingCandidate = true;
		Rules.Add(CrossingCandidate);

		FJungleCreekRiparianRuleContract MuddyDeadfall;
		MuddyDeadfall.Zone = EJungleCreekRiparianWetValleyZone::MuddyDeadfallSoftBlocker;
		MuddyDeadfall.EvidenceCue = EJungleCreekEvidenceCue::DeadfallTangle;
		MuddyDeadfall.RuleKind = EJunglePCGSpawnRuleKind::DeadfallPlaceholder;
		MuddyDeadfall.FrictionClass = EJunglePCGTerrainFrictionClass::ReadablePlantFriction;
		MuddyDeadfall.RequiredMaskNames = { TEXT("wetness"), TEXT("soft_blocker"), TEXT("footing_risk") };
		MuddyDeadfall.MaxInstancesPerHectare = 52.0f;
		MuddyDeadfall.MinimumSpacingMeters = 4.0f;
		Rules.Add(MuddyDeadfall);

		FJungleCreekRiparianRuleContract SoundAnchor;
		SoundAnchor.Zone = EJungleCreekRiparianWetValleyZone::WaterSoundAnchor;
		SoundAnchor.EvidenceCue = EJungleCreekEvidenceCue::ChannelSound;
		SoundAnchor.RuleKind = EJunglePCGSpawnRuleKind::SoundAnchorPlaceholder;
		SoundAnchor.FrictionClass = EJunglePCGTerrainFrictionClass::Open;
		SoundAnchor.RequiredMaskNames = { TEXT("creek_distance"), TEXT("sound_propagation"), TEXT("visibility") };
		SoundAnchor.MaxInstancesPerHectare = 12.0f;
		SoundAnchor.MinimumSpacingMeters = 12.0f;
		Rules.Add(SoundAnchor);

		FJungleCreekRiparianRuleContract FalseOpening;
		FalseOpening.Zone = EJungleCreekRiparianWetValleyZone::FalseCreekAffordance;
		FalseOpening.EvidenceCue = EJungleCreekEvidenceCue::FalseBankOpening;
		FalseOpening.RuleKind = EJunglePCGSpawnRuleKind::FalseAffordanceCuePlaceholder;
		FalseOpening.FrictionClass = EJunglePCGTerrainFrictionClass::FalseAffordance;
		FalseOpening.RequiredMaskNames = { TEXT("false_affordance"), TEXT("creek_distance"), TEXT("soft_blocker") };
		FalseOpening.MaxInstancesPerHectare = 10.0f;
		FalseOpening.MinimumSpacingMeters = 14.0f;
		FalseOpening.bCanCreateFalseAffordance = true;
		Rules.Add(FalseOpening);

		return Rules;
	}

	static TArray<FString> RequiredValidationOutputNames()
	{
		return {
			TEXT("creek_riparian_zone"),
			TEXT("creek_crossing_candidate"),
			TEXT("creek_bank_affordance"),
			TEXT("wet_valley_soft_blocker"),
			TEXT("water_sound_anchor"),
			TEXT("creek_false_affordance"),
			TEXT("leech_tick_placeholder_risk_note"),
			TEXT("creek_reach_continuity"),
			TEXT("creek_crossing_exit_plausible"),
			TEXT("creek_rain_response_debug"),
		};
	}
};
