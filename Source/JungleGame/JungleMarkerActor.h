#pragma once

#include "CoreMinimal.h"
#include "JungleInteractableActor.h"
#include "JungleMarkerActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class JUNGLEGAME_API AJungleMarkerActor : public AJungleInteractableActor
{
	GENERATED_BODY()

public:
	AJungleMarkerActor();

	virtual void Interact(AActor* Interactor) override;
	virtual FText GetInteractionText() const override;

	void RevealMarker();
	bool IsRevealed() const;

private:
	UPROPERTY(VisibleAnywhere, Category = "Jungle|Marker")
	TObjectPtr<UStaticMeshComponent> MarkerMesh;

	bool bRevealed = false;
};
