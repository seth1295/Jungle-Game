#pragma once

#include "CoreMinimal.h"
#include "JungleEcosystemMaskSpec.h"

/** Developer-only mask visualization modes. These are not player navigation UI. */
enum class EJungleEcosystemDebugView : uint8
{
	RawMask,
	TraversalClassification,
	ComponentContinuity,
	FirstPersonProbe,
};

/** Developer-only probe result for validating why a point received a mask classification. */
struct FJungleEcosystemDebugProbe
{
	FString MaskName;
	float RawValue = 0.0f;
	EJungleTraversalMaskClass TraversalClass = EJungleTraversalMaskClass::Open;
	FString Reason;

	bool IsValidRawValue() const
	{
		return FJungleEcosystemMaskSpec::IsNormalized(RawValue);
	}
};

struct FJungleEcosystemDebugSpec
{
	static constexpr const TCHAR* DeveloperOnlyNotice = TEXT("Developer-only ecosystem debug data; never expose as player navigation UI.");

	static bool IsPlayerFacingView(EJungleEcosystemDebugView View)
	{
		return false;
	}

	static TArray<FString> RequiredValidationViews()
	{
		return {
			TEXT("raw_mask_ranges"),
			TEXT("traversal_classification"),
			TEXT("component_border_continuity"),
			TEXT("first_person_probe"),
		};
	}
};
