#include "JungleCrossingActor.h"

#include "Components/StaticMeshComponent.h"
#include "JungleGame.h"
#include "UObject/ConstructorHelpers.h"

AJungleCrossingActor::AJungleCrossingActor()
{
	CrossingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrossingMesh"));
	CrossingMesh->SetupAttachment(GetRootComponent());
	CrossingMesh->SetRelativeScale3D(FVector(3.5f, 0.35f, 0.18f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshRef.Succeeded())
	{
		CrossingMesh->SetStaticMesh(MeshRef.Object);
	}
}

void AJungleCrossingActor::Interact(AActor* Interactor)
{
	Super::Interact(Interactor);
	UE_LOG(LogJungleGame, Display, TEXT("Crossing inspected. Changed=%s"), bChanged ? TEXT("true") : TEXT("false"));
}

FText AJungleCrossingActor::GetInteractionText() const
{
	return bChanged ? FText::FromString(TEXT("Inspect tied crossing")) : FText::FromString(TEXT("Inspect crossing"));
}

void AJungleCrossingActor::ApplyStateChange()
{
	if (bChanged)
	{
		return;
	}

	bChanged = true;
	UpdateVisualState();
	UE_LOG(LogJungleGame, Display, TEXT("Cell 0 crossing state changed while player was away."));
}

bool AJungleCrossingActor::HasChanged() const
{
	return bChanged;
}

void AJungleCrossingActor::UpdateVisualState()
{
	CrossingMesh->SetRelativeRotation(bChanged ? FRotator(0.0f, 12.0f, 0.0f) : FRotator::ZeroRotator);
	CrossingMesh->SetRelativeLocation(bChanged ? FVector(0.0f, 60.0f, 0.0f) : FVector::ZeroVector);
}
