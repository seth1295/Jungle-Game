#pragma once

#include "CoreMinimal.h"

/** Developer-only full-size world authority status for PR5 Batch 002. */
enum class EJGFullSizeWorldAuthorityStatus : uint8
{
	NotEstablished,
	AuthorityShell,
	ProjectOwnedMapPending,
	ProjectOwnedMapLoaded,
};

enum class EJGLegacyWorldPolicy : uint8
{
	TemplateSourceOnly,
	LegacyValidationSlice,
	ReviewAnchorOnly,
	ForbiddenAsFullWorldAuthority,
};

struct FJGFullSizeWorldAuthorityReviewPoint
{
	FName StableId;
	FVector Location = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;
	FName LandformIntent;
	bool bDeveloperOnly = true;
	bool bPlayerNavigationAid = false;
};

struct FJGFullSizeWorldAuthoritySpec
{
	static constexpr int32 AuthoritySpecVersion = 1;

	static FName AuthorityId()
	{
		return TEXT("JG_FULL_WORLD_001");
	}

	static FName IntendedWorldAssetPath()
	{
		return TEXT("/Game/JungleGame/Maps/WLD_JungleIsland_Full");
	}

	static FName CurrentBridgeWorldPath()
	{
		return TEXT("/Engine/Maps/Templates/OpenWorld");
	}

	static FName AuthorityPurpose()
	{
		return TEXT("FullSizePlayableWorldAuthority");
	}

	static EJGFullSizeWorldAuthorityStatus CurrentStatus()
	{
		return EJGFullSizeWorldAuthorityStatus::AuthorityShell;
	}

	static EJGLegacyWorldPolicy TemplateOpenWorldPolicy()
	{
		return EJGLegacyWorldPolicy::TemplateSourceOnly;
	}

	static EJGLegacyWorldPolicy Cell0Policy()
	{
		return EJGLegacyWorldPolicy::ReviewAnchorOnly;
	}

	static bool AllowsPlayerFacingNavigationUI()
	{
		return false;
	}

	static bool AllowsCell0ToDefineFullWorld()
	{
		return false;
	}

	static bool AllowsTemplateOpenWorldAsFinalAuthority()
	{
		return false;
	}

	static bool IsExpectedBridgeWorld(const FString& WorldPath)
	{
		return WorldPath.Contains(TEXT("/Engine/Maps/Templates/OpenWorld"))
			|| WorldPath.Contains(TEXT("OpenWorld"));
	}

	static TArray<FJGFullSizeWorldAuthorityReviewPoint> DefaultReviewPoints()
	{
		return {
			{ TEXT("FSW_RP_Cell0Bridge"), FVector(204932.646f, -96678.529f, 1200.0f), FRotator(0.0f, 34.0f, 0.0f), TEXT("LegacyCell0ReviewAnchor"), true, false },
			{ TEXT("FSW_RP_FutureCoast"), FVector(196000.0f, -104000.0f, 1300.0f), FRotator(0.0f, 62.0f, 0.0f), TEXT("FutureCoastReview"), true, false },
			{ TEXT("FSW_RP_FutureRidge"), FVector(212000.0f, -92000.0f, 2200.0f), FRotator(0.0f, -28.0f, 0.0f), TEXT("FutureRidgeReview"), true, false },
			{ TEXT("FSW_RP_FutureCreek"), FVector(207500.0f, -99200.0f, 1100.0f), FRotator(0.0f, 15.0f, 0.0f), TEXT("FutureCreekReview"), true, false },
			{ TEXT("FSW_RP_FutureBasin"), FVector(202000.0f, -91500.0f, 900.0f), FRotator(0.0f, 118.0f, 0.0f), TEXT("FutureBasinReview"), true, false },
		};
	}

	static bool ReviewPointsAreDeveloperOnly()
	{
		for (const FJGFullSizeWorldAuthorityReviewPoint& Point : DefaultReviewPoints())
		{
			if (!Point.bDeveloperOnly || Point.bPlayerNavigationAid)
			{
				return false;
			}
		}

		return true;
	}
};
