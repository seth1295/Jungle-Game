#include "JungleCell0Director.h"

#include "Engine/StaticMeshActor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "JungleCrossingActor.h"
#include "JungleFireActor.h"
#include "JungleGame.h"
#include "JungleMarkerActor.h"
#include "JungleWatcherCueActor.h"
#include "Kismet/GameplayStatics.h"
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
	MovePlayerToCellEntryPoint();

	FireActor = GetWorld()->SpawnActor<AJungleFireActor>(AJungleFireActor::StaticClass(), ToWorld(FVector(320.0f, 0.0f, -40.0f)), ToWorldRotation());
	CrossingActor = GetWorld()->SpawnActor<AJungleCrossingActor>(AJungleCrossingActor::StaticClass(), ToWorld(FVector(920.0f, 0.0f, -10.0f)), ToWorldRotation());
	MarkerActor = GetWorld()->SpawnActor<AJungleMarkerActor>(AJungleMarkerActor::StaticClass(), ToWorld(FVector(1040.0f, 170.0f, -25.0f)), ToWorldRotation(25.0f));
	CueActor = GetWorld()->SpawnActor<AJungleWatcherCueActor>(AJungleWatcherCueActor::StaticClass(), ToWorld(FVector(1300.0f, -420.0f, 130.0f)), ToWorldRotation());

	AddCube(FVector(320.0f, 0.0f, -95.0f), FVector(9.0f, 5.0f, 0.1f), TEXT("Cell0CampGround"));
	AddCube(FVector(520.0f, -260.0f, 40.0f), FVector(0.55f, 0.55f, 3.2f), TEXT("Cell0TrunkLeft"));
	AddCube(FVector(520.0f, 260.0f, 40.0f), FVector(0.55f, 0.55f, 3.2f), TEXT("Cell0TrunkRight"));
	AddCube(FVector(700.0f, 0.0f, 90.0f), FVector(0.25f, 5.0f, 1.8f), TEXT("Cell0VisibilityWall"));
	AddCube(FVector(1050.0f, 0.0f, -90.0f), FVector(5.0f, 1.6f, 0.08f), TEXT("Cell0CreekBed"));

	UE_LOG(LogJungleGame, Display, TEXT("Jungle Cell 0 integrated world location spawned at %s facing %.1f degrees using %s anchor."), *CellOrigin.ToString(), CellRotation.Yaw, AnchorMode == EJungleCell0AnchorMode::PlacedWorldLocation ? TEXT("placed-world") : TEXT("player-relative-debug"));
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

void AJungleCell0Director::AddCube(const FVector& LocalLocation, const FVector& Scale, const FName Name)
{
	if (!CubeMesh)
	{
		UE_LOG(LogJungleGame, Warning, TEXT("CubeMesh missing; skipping blockout actor '%s'."), *Name.ToString());
		return;
	}

	AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), ToWorld(LocalLocation), ToWorldRotation());
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
