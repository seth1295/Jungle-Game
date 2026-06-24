#include "JungleInteractableActor.h"

AJungleInteractableActor::AJungleInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
}

void AJungleInteractableActor::Interact(AActor* Interactor)
{
}

FText AJungleInteractableActor::GetInteractionText() const
{
	return FText::FromString(TEXT("Interact"));
}
