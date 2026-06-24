#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JungleInteractableActor.generated.h"

UCLASS(Abstract)
class JUNGLEGAME_API AJungleInteractableActor : public AActor
{
	GENERATED_BODY()

public:
	AJungleInteractableActor();

	virtual void Interact(AActor* Interactor);
	virtual FText GetInteractionText() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jungle|Interactable")
	TObjectPtr<USceneComponent> SceneRoot;
};
