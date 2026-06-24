#pragma once

#include "CoreMinimal.h"
#include "JungleInteractableActor.h"
#include "JungleCrossingActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class JUNGLEGAME_API AJungleCrossingActor : public AJungleInteractableActor
{
	GENERATED_BODY()

public:
	AJungleCrossingActor();

	virtual void Interact(AActor* Interactor) override;
	virtual FText GetInteractionText() const override;

	void ApplyStateChange();
	bool HasChanged() const;

private:
	void UpdateVisualState();

	UPROPERTY(VisibleAnywhere, Category = "Jungle|Crossing")
	TObjectPtr<UStaticMeshComponent> CrossingMesh;

	bool bChanged = false;
};
