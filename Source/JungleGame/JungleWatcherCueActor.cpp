#include "JungleWatcherCueActor.h"

#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "JungleGame.h"
#include "UObject/ConstructorHelpers.h"

AJungleWatcherCueActor::AJungleWatcherCueActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	CueMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CueMesh"));
	CueMesh->SetupAttachment(SceneRoot);
	CueMesh->SetRelativeScale3D(FVector(0.45f, 0.45f, 0.45f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (MeshRef.Succeeded())
	{
		CueMesh->SetStaticMesh(MeshRef.Object);
	}

	CueLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("CueLight"));
	CueLight->SetupAttachment(CueMesh);
	CueLight->SetAttenuationRadius(300.0f);
	CueLight->SetIntensity(0.0f);

	SetActorHiddenInGame(true);
}

void AJungleWatcherCueActor::RevealCue()
{
	bRevealed = true;
	SetActorHiddenInGame(false);
	CueLight->SetIntensity(900.0f);
	UE_LOG(LogJungleGame, Display, TEXT("Distant watcher cue revealed without confrontation."));
}

bool AJungleWatcherCueActor::IsRevealed() const
{
	return bRevealed;
}
