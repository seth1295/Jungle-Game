#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JungleCell0Director.generated.h"

class AJungleCrossingActor;
class AJungleFireActor;
class AJungleMarkerActor;
class AJungleWatcherCueActor;
class UStaticMesh;

UENUM(BlueprintType)
enum class EJungleCell0AnchorMode : uint8
{
	PlacedWorldLocation,
	PlayerRelativeDebug,
};

UCLASS()
class JUNGLEGAME_API AJungleCell0Director : public AActor
{
	GENERATED_BODY()

public:
	AJungleCell0Director();

	void ConfigureLargeWorldPlacement(const FVector& NewCellOrigin, const FRotator& NewCellRotation, bool bMovePlayerToEntryPoint);

protected:
	virtual void BeginPlay() override;

private:
	void BuildCell();
	void BuildFirstPlayableTerrain();
	void StartRain();
	void ChangeCrossing();
	void ShowCue();
	void LogPlayableVisibilityDiagnostics();
	void ScheduleVisualSmokeIfRequested();
	void MovePlayerToVisualSmokeView();
	void TakeVisualSmokeShot();
	void ExitAfterVisualSmokeShot();
	void ResolveCellAnchor();
	void CapturePlayerAnchor();
	void CapturePlacedWorldAnchor();
	void MovePlayerToCellEntryPoint();
	FVector ToWorld(const FVector& LocalLocation) const;
	FRotator ToWorldRotation(float LocalYawDegrees = 0.0f) const;
	void AddCube(const FVector& LocalLocation, const FVector& Scale, const FName Name, float LocalYawDegrees = 0.0f);

	UPROPERTY()
	TObjectPtr<AJungleFireActor> FireActor;

	UPROPERTY()
	TObjectPtr<AJungleCrossingActor> CrossingActor;

	UPROPERTY()
	TObjectPtr<AJungleMarkerActor> MarkerActor;

	UPROPERTY()
	TObjectPtr<AJungleWatcherCueActor> CueActor;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY(EditAnywhere, Category = "Jungle|Cell 0 Integration")
	EJungleCell0AnchorMode AnchorMode = EJungleCell0AnchorMode::PlacedWorldLocation;

	UPROPERTY(EditAnywhere, Category = "Jungle|Cell 0 Integration")
	bool bMovePlayerToEntryOnBeginPlay = true;

	UPROPERTY(EditAnywhere, Category = "Jungle|Cell 0 Integration")
	FVector PlayerEntryLocalLocation = FVector(-180.0f, -180.0f, 0.0f);

	FVector CellOrigin = FVector::ZeroVector;
	FRotator CellRotation = FRotator::ZeroRotator;
	bool bHasConfiguredPlacement = false;
	int32 PlayerEntryMoveAttemptsRemaining = 30;
};
