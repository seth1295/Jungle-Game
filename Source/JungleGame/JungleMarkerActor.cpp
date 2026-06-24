#include "JungleMarkerActor.h"

#include "Components/StaticMeshComponent.h"
#include "JungleGame.h"
#include "UObject/ConstructorHelpers.h"

AJungleMarkerActor::AJungleMarkerActor()
{
	MarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerMesh"));
	MarkerMesh->SetupAttachment(GetRootComponent());
	MarkerMesh->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.08f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshRef.Succeeded())
	{
		MarkerMesh->SetStaticMesh(MeshRef.Object);
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AJungleMarkerActor::Interact(AActor* Interactor)
{
	Super::Interact(Interactor);
	UE_LOG(LogJungleGame, Display, TEXT("Hand-made marker inspected."));
}

FText AJungleMarkerActor::GetInteractionText() const
{
	return FText::FromString(TEXT("Inspect arranged marker"));
}

void AJungleMarkerActor::RevealMarker()
{
	bRevealed = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	UE_LOG(LogJungleGame, Display, TEXT("Arranged marker revealed near crossing."));
}

bool AJungleMarkerActor::IsRevealed() const
{
	return bRevealed;
}
