#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JungleCell0Director.generated.h"

class AJungleCrossingActor;
class AJungleFireActor;
class AJungleMarkerActor;
class AJungleWatcherCueActor;
class UStaticMesh;

UCLASS()
class JUNGLEGAME_API AJungleCell0Director : public AActor
{
	GENERATED_BODY()

public:
	AJungleCell0Director();

protected:
	virtual void BeginPlay() override;

private:
	void BuildCell();
	void StartRain();
	void ChangeCrossing();
	void ShowCue();
	void AddCube(const FVector& Location, const FVector& Scale, const FName Name);

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
};
