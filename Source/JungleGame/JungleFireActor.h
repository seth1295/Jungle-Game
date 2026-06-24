#pragma once

#include "CoreMinimal.h"
#include "JungleInteractableActor.h"
#include "JungleFireActor.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;

UCLASS()
class JUNGLEGAME_API AJungleFireActor : public AJungleInteractableActor
{
	GENERATED_BODY()

public:
	AJungleFireActor();

	virtual void Tick(float DeltaSeconds) override;
	virtual void Interact(AActor* Interactor) override;
	virtual FText GetInteractionText() const override;

	void SetRaining(bool bNewRaining);
	float GetEmberLevel() const;

protected:
	virtual void BeginPlay() override;

private:
	void UpdateVisualState();

	UPROPERTY(VisibleAnywhere, Category = "Jungle|Fire")
	TObjectPtr<UStaticMeshComponent> FireMesh;

	UPROPERTY(VisibleAnywhere, Category = "Jungle|Fire")
	TObjectPtr<UPointLightComponent> FireLight;

	UPROPERTY(EditAnywhere, Category = "Jungle|Fire")
	float EmberLevel = 0.65f;

	UPROPERTY(EditAnywhere, Category = "Jungle|Fire")
	float DryDecayPerSecond = 0.012f;

	UPROPERTY(EditAnywhere, Category = "Jungle|Fire")
	float RainDecayPerSecond = 0.045f;

	UPROPERTY(EditAnywhere, Category = "Jungle|Fire")
	float TendAmount = 0.45f;

	bool bRaining = false;
};
