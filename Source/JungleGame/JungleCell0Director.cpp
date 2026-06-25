#include "JungleCell0Director.h"

#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "JungleCrossingActor.h"
#include "JungleFireActor.h"
#include "JungleGame.h"
#include "JungleMarkerActor.h"
#include "JungleWatcherCueActor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AJungleCell0Director::AJungleCell0Director()
{
	PrimaryActorTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshRef.Succeeded())
	{
		CubeMesh = MeshRef.Object;
	}
}

void AJungleCell0Director::BeginPlay()
{
	Super::BeginPlay();
	BuildCell();

	GetWorldTimerManager().SetTimerForNextTick(this, &AJungleCell0Director::StartRain);
	FTimerHandle DiagnosticTimer;
	GetWorldTimerManager().SetTimer(DiagnosticTimer, this, &AJungleCell0Director::LogPlayableVisibilityDiagnostics, 1.0f, false);
	ScheduleVisualSmokeIfRequested();
	FTimerHandle CrossingTimer;
	GetWorldTimerManager().SetTimer(CrossingTimer, this, &AJungleCell0Director::ChangeCrossing, 8.0f, false);
	FTimerHandle CueTimer;
	GetWorldTimerManager().SetTimer(CueTimer, this, &AJungleCell0Director::ShowCue, 14.0f, false);
}

void AJungleCell0Director::ConfigureLargeWorldPlacement(const FVector& NewCellOrigin, const FRotator& NewCellRotation, bool bMovePlayerToEntryPoint)
{
	CellOrigin = NewCellOrigin;
	CellRotation = FRotator(0.0f, NewCellRotation.Yaw, 0.0f);
	AnchorMode = EJungleCell0AnchorMode::PlacedWorldLocation;
	bMovePlayerToEntryOnBeginPlay = bMovePlayerToEntryPoint;
	PlayerEntryMoveAttemptsRemaining = 30;
	bHasConfiguredPlacement = true;
}

void AJungleCell0Director::BuildCell()
{
	ResolveCellAnchor();
	BuildFirstPlayableTerrain();
	MovePlayerToCellEntryPoint();

	FireActor = GetWorld()->SpawnActor<AJungleFireActor>(AJungleFireActor::StaticClass(), ToWorld(FVector(320.0f, 0.0f, -40.0f)), ToWorldRotation());
	CrossingActor = GetWorld()->SpawnActor<AJungleCrossingActor>(AJungleCrossingActor::StaticClass(), ToWorld(FVector(920.0f, 0.0f, -10.0f)), ToWorldRotation());
	MarkerActor = GetWorld()->SpawnActor<AJungleMarkerActor>(AJungleMarkerActor::StaticClass(), ToWorld(FVector(1040.0f, 170.0f, -25.0f)), ToWorldRotation(25.0f));
	CueActor = GetWorld()->SpawnActor<AJungleWatcherCueActor>(AJungleWatcherCueActor::StaticClass(), ToWorld(FVector(1560.0f, -420.0f, 130.0f)), ToWorldRotation());

	UE_LOG(LogJungleGame, Display, TEXT("Jungle Cell 0 playable source-authored slice spawned at %s facing %.1f degrees using %s anchor."), *CellOrigin.ToString(), CellRotation.Yaw, AnchorMode == EJungleCell0AnchorMode::PlacedWorldLocation ? TEXT("placed-world") : TEXT("player-relative-debug"));
}

void AJungleCell0Director::BuildFirstPlayableTerrain()
{
	// Source-authored first-playable geometry. This deliberately avoids committing a binary .umap while still giving
	// packaged builds a physical loop to walk, read, and validate: shelter -> creek -> marker -> watcher cue -> exit ridge.
	AddCube(FVector(300.0f, 0.0f, -95.0f), FVector(16.0f, 8.0f, 0.1f), TEXT("Cell0CampGround"));
	AddCube(FVector(900.0f, 0.0f, -96.0f), FVector(11.5f, 5.0f, 0.08f), TEXT("Cell0CreekApproachGround"));
	AddCube(FVector(1360.0f, -120.0f, -86.0f), FVector(8.5f, 4.2f, 0.16f), TEXT("Cell0FarBankGround"), -10.0f);
	AddCube(FVector(1780.0f, -380.0f, -62.0f), FVector(7.5f, 3.2f, 0.22f), TEXT("Cell0ExitRidgeGround"), -18.0f);

	AddCube(FVector(120.0f, -520.0f, 35.0f), FVector(4.2f, 0.35f, 1.4f), TEXT("Cell0LeftJungleEdge"), -8.0f);
	AddCube(FVector(180.0f, 520.0f, 38.0f), FVector(4.4f, 0.35f, 1.5f), TEXT("Cell0RightJungleEdge"), 10.0f);
	AddCube(FVector(-260.0f, -60.0f, 30.0f), FVector(1.2f, 4.8f, 1.6f), TEXT("Cell0RearJungleEdge"), 86.0f);
	AddCube(FVector(-130.0f, -340.0f, 75.0f), FVector(0.42f, 0.42f, 2.8f), TEXT("Cell0RearTreeA"), -10.0f);
	AddCube(FVector(-90.0f, 350.0f, 82.0f), FVector(0.45f, 0.45f, 3.0f), TEXT("Cell0RearTreeB"), 14.0f);

	AddCube(FVector(520.0f, -260.0f, 40.0f), FVector(0.55f, 0.55f, 3.2f), TEXT("Cell0TrunkLeft"));
	AddCube(FVector(520.0f, 260.0f, 40.0f), FVector(0.55f, 0.55f, 3.2f), TEXT("Cell0TrunkRight"));
	AddCube(FVector(450.0f, -470.0f, 70.0f), FVector(0.45f, 0.45f, 2.8f), TEXT("Cell0ShelterTreeA"), 8.0f);
	AddCube(FVector(610.0f, 430.0f, 75.0f), FVector(0.5f, 0.5f, 3.0f), TEXT("Cell0ShelterTreeB"), -15.0f);
	AddCube(FVector(880.0f, -360.0f, 85.0f), FVector(0.45f, 0.45f, 3.1f), TEXT("Cell0CreekTreeA"), 4.0f);
	AddCube(FVector(1260.0f, 280.0f, 90.0f), FVector(0.45f, 0.45f, 3.4f), TEXT("Cell0FarBankTreeA"), -12.0f);
	AddCube(FVector(1710.0f, -610.0f, 95.0f), FVector(0.5f, 0.5f, 3.6f), TEXT("Cell0ExitRidgeTreeA"), 18.0f);

	AddCube(FVector(700.0f, 0.0f, 90.0f), FVector(0.25f, 5.0f, 1.8f), TEXT("Cell0VisibilityWall"));
	AddCube(FVector(1120.0f, -10.0f, -92.0f), FVector(5.4f, 1.6f, 0.08f), TEXT("Cell0CreekBed"));
	AddCube(FVector(1140.0f, -215.0f, -66.0f), FVector(4.7f, 0.28f, 0.18f), TEXT("Cell0NearCreekBank"), -5.0f);
	AddCube(FVector(1140.0f, 215.0f, -66.0f), FVector(4.7f, 0.28f, 0.18f), TEXT("Cell0FarCreekBank"), 5.0f);

	AddCube(FVector(1380.0f, -430.0f, 22.0f), FVector(2.0f, 0.28f, 1.4f), TEXT("Cell0WatcherSightlineBreak"), -22.0f);
	AddCube(FVector(1760.0f, -170.0f, 18.0f), FVector(0.35f, 2.4f, 1.1f), TEXT("Cell0ExitRidgeShoulder"), 28.0f);
	AddCube(FVector(2100.0f, -620.0f, 110.0f), FVector(0.8f, 0.8f, 4.2f), TEXT("Cell0DistantLandmarkTrunk"), 0.0f);
}

void AJungleCell0Director::StartRain()
{
	if (FireActor)
	{
		FireActor->SetRaining(true);
	}
	UE_LOG(LogJungleGame, Display, TEXT("Rain pressure started."));
}

void AJungleCell0Director::ChangeCrossing()
{
	if (CrossingActor)
	{
		CrossingActor->ApplyStateChange();
	}
	if (MarkerActor)
	{
		MarkerActor->RevealMarker();
	}
}

void AJungleCell0Director::ShowCue()
{
	if (CueActor)
	{
		CueActor->RevealCue();
	}
}

void AJungleCell0Director::LogPlayableVisibilityDiagnostics()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerController || !PlayerPawn)
	{
		UE_LOG(LogJungleGame, Warning, TEXT("Cell 0 visibility diagnostic skipped: missing player controller or pawn."));
		return;
	}

	FVector ViewLocation = FVector::ZeroVector;
	FRotator ViewRotation = FRotator::ZeroRotator;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector DownStart = PlayerPawn->GetActorLocation() + FVector(0.0f, 0.0f, 120.0f);
	const FVector DownEnd = DownStart - FVector(0.0f, 0.0f, 2000.0f);
	const FRotator ForwardRotation = ViewRotation;
	const FRotator RightRotation(ViewRotation.Pitch, ViewRotation.Yaw + 90.0f, 0.0f);
	const FRotator BackRotation(ViewRotation.Pitch, ViewRotation.Yaw + 180.0f, 0.0f);
	const FRotator LeftRotation(ViewRotation.Pitch, ViewRotation.Yaw - 90.0f, 0.0f);
	const FVector ForwardEnd = ViewLocation + ForwardRotation.Vector() * 4000.0f;
	const FVector RightEnd = ViewLocation + RightRotation.Vector() * 4000.0f;
	const FVector BackEnd = ViewLocation + BackRotation.Vector() * 4000.0f;
	const FVector LeftEnd = ViewLocation + LeftRotation.Vector() * 4000.0f;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Cell0VisibilityDiagnostic), false, PlayerPawn);

	FHitResult DownHit;
	const bool bDownHit = GetWorld()->LineTraceSingleByChannel(DownHit, DownStart, DownEnd, ECC_Visibility, Params);
	FHitResult ForwardHit;
	const bool bForwardHit = GetWorld()->LineTraceSingleByChannel(ForwardHit, ViewLocation, ForwardEnd, ECC_Visibility, Params);
	FHitResult RightHit;
	const bool bRightHit = GetWorld()->LineTraceSingleByChannel(RightHit, ViewLocation, RightEnd, ECC_Visibility, Params);
	FHitResult BackHit;
	const bool bBackHit = GetWorld()->LineTraceSingleByChannel(BackHit, ViewLocation, BackEnd, ECC_Visibility, Params);
	FHitResult LeftHit;
	const bool bLeftHit = GetWorld()->LineTraceSingleByChannel(LeftHit, ViewLocation, LeftEnd, ECC_Visibility, Params);

	UE_LOG(LogJungleGame, Display, TEXT("Cell 0 visibility diagnostic: pawn=%s view=%s rot=%s down_hit=%s down_actor=%s down_loc=%s forward_hit=%s forward_actor=%s forward_loc=%s right_hit=%s right_actor=%s right_loc=%s back_hit=%s back_actor=%s back_loc=%s left_hit=%s left_actor=%s left_loc=%s"),
		*PlayerPawn->GetActorLocation().ToString(),
		*ViewLocation.ToString(),
		*ViewRotation.ToString(),
		bDownHit ? TEXT("true") : TEXT("false"),
		DownHit.GetActor() ? *DownHit.GetActor()->GetName() : TEXT("none"),
		*DownHit.Location.ToString(),
		bForwardHit ? TEXT("true") : TEXT("false"),
		ForwardHit.GetActor() ? *ForwardHit.GetActor()->GetName() : TEXT("none"),
		*ForwardHit.Location.ToString(),
		bRightHit ? TEXT("true") : TEXT("false"),
		RightHit.GetActor() ? *RightHit.GetActor()->GetName() : TEXT("none"),
		*RightHit.Location.ToString(),
		bBackHit ? TEXT("true") : TEXT("false"),
		BackHit.GetActor() ? *BackHit.GetActor()->GetName() : TEXT("none"),
		*BackHit.Location.ToString(),
		bLeftHit ? TEXT("true") : TEXT("false"),
		LeftHit.GetActor() ? *LeftHit.GetActor()->GetName() : TEXT("none"),
		*LeftHit.Location.ToString());
}

void AJungleCell0Director::ScheduleVisualSmokeIfRequested()
{
	if (!FParse::Param(FCommandLine::Get(), TEXT("JungleVisualSmoke")))
	{
		return;
	}

	MovePlayerToVisualSmokeView();
	FTimerHandle ShotTimer;
	GetWorldTimerManager().SetTimer(ShotTimer, this, &AJungleCell0Director::TakeVisualSmokeShot, 2.0f, false);
	UE_LOG(LogJungleGame, Display, TEXT("Cell 0 visual smoke capture scheduled."));
}

void AJungleCell0Director::MovePlayerToVisualSmokeView()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerPawn || !PlayerController)
	{
		UE_LOG(LogJungleGame, Warning, TEXT("Cell 0 visual smoke view skipped: missing player controller or pawn."));
		return;
	}

	const FVector ViewLocalLocation = FVector(120.0f, -80.0f, 0.0f);
	const float ViewYaw = CellRotation.Yaw + 8.0f;
	const FRotator PawnRotation = FRotator(0.0f, ViewYaw, 0.0f);
	const FRotator ViewRotation = FRotator(-7.0f, ViewYaw, 0.0f);
	PlayerPawn->SetActorLocationAndRotation(ToWorld(ViewLocalLocation), PawnRotation, false, nullptr, ETeleportType::TeleportPhysics);
	PlayerController->SetControlRotation(ViewRotation);
	UE_LOG(LogJungleGame, Display, TEXT("Cell 0 visual smoke view set at %s with rotation %s."), *PlayerPawn->GetActorLocation().ToString(), *ViewRotation.ToString());
}

void AJungleCell0Director::TakeVisualSmokeShot()
{
	if (!GEngine || !GetWorld())
	{
		UE_LOG(LogJungleGame, Warning, TEXT("Cell 0 visual smoke shot skipped: engine or world missing."));
		return;
	}

	UE_LOG(LogJungleGame, Display, TEXT("Cell 0 visual smoke shot requested."));
	GEngine->Exec(GetWorld(), TEXT("HighResShot 1920x1080"));
	FTimerHandle ExitTimer;
	GetWorldTimerManager().SetTimer(ExitTimer, this, &AJungleCell0Director::ExitAfterVisualSmokeShot, 1.5f, false);
}

void AJungleCell0Director::ExitAfterVisualSmokeShot()
{
	if (GEngine && GetWorld())
	{
		GEngine->Exec(GetWorld(), TEXT("quit"));
	}
}

void AJungleCell0Director::ResolveCellAnchor()
{
	if (AnchorMode == EJungleCell0AnchorMode::PlayerRelativeDebug)
	{
		CapturePlayerAnchor();
		return;
	}

	CapturePlacedWorldAnchor();
}

void AJungleCell0Director::CapturePlayerAnchor()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (PlayerPawn)
	{
		CellOrigin = PlayerPawn->GetActorLocation();
		const float Yaw = PlayerController ? PlayerController->GetControlRotation().Yaw : PlayerPawn->GetActorRotation().Yaw;
		CellRotation = FRotator(0.0f, Yaw, 0.0f);
		return;
	}

	CellOrigin = GetActorLocation();
	CellRotation = FRotator(0.0f, GetActorRotation().Yaw, 0.0f);
}

void AJungleCell0Director::CapturePlacedWorldAnchor()
{
	if (!bHasConfiguredPlacement)
	{
		CellOrigin = GetActorLocation();
		CellRotation = FRotator(0.0f, GetActorRotation().Yaw, 0.0f);
	}
}

void AJungleCell0Director::MovePlayerToCellEntryPoint()
{
	if (!bMovePlayerToEntryOnBeginPlay || AnchorMode != EJungleCell0AnchorMode::PlacedWorldLocation)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		if (PlayerEntryMoveAttemptsRemaining > 0)
		{
			--PlayerEntryMoveAttemptsRemaining;
			FTimerHandle RetryTimer;
			GetWorldTimerManager().SetTimer(RetryTimer, this, &AJungleCell0Director::MovePlayerToCellEntryPoint, 0.1f, false);
		}
		else
		{
			bMovePlayerToEntryOnBeginPlay = false;
			UE_LOG(LogJungleGame, Warning, TEXT("Cell 0 entry move skipped because player pawn was not available after retries."));
		}
		return;
	}

	const FVector EntryLocation = ToWorld(PlayerEntryLocalLocation);
	const FRotator EntryRotation = ToWorldRotation(0.0f);
	PlayerPawn->SetActorLocationAndRotation(EntryLocation, EntryRotation, false, nullptr, ETeleportType::TeleportPhysics);

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		PlayerController->SetControlRotation(EntryRotation);
	}

	bMovePlayerToEntryOnBeginPlay = false;
	PlayerEntryMoveAttemptsRemaining = 0;
	UE_LOG(LogJungleGame, Display, TEXT("Player moved to Cell 0 large-world entry at %s."), *EntryLocation.ToString());
}

FVector AJungleCell0Director::ToWorld(const FVector& LocalLocation) const
{
	return CellOrigin + CellRotation.RotateVector(LocalLocation);
}

FRotator AJungleCell0Director::ToWorldRotation(float LocalYawDegrees) const
{
	return FRotator(0.0f, CellRotation.Yaw + LocalYawDegrees, 0.0f);
}

void AJungleCell0Director::AddCube(const FVector& LocalLocation, const FVector& Scale, const FName Name, float LocalYawDegrees)
{
	if (!CubeMesh)
	{
		UE_LOG(LogJungleGame, Warning, TEXT("CubeMesh missing; skipping blockout actor '%s'."), *Name.ToString());
		return;
	}

	AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), ToWorld(LocalLocation), ToWorldRotation(LocalYawDegrees));
	if (!MeshActor)
	{
		return;
	}

#if WITH_EDITOR
	MeshActor->SetActorLabel(Name.ToString());
#endif
	MeshActor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
	MeshActor->SetActorScale3D(Scale);
}
