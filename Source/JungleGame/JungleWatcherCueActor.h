#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JungleWatcherCueActor.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;

UCLASS()
class JUNGLEGAME_API AJungleWatcherCueActor : public AActor
{
	GENERATED_BODY()

public:
	AJungleWatcherCueActor();

	void RevealCue();
	bool IsRevealed() const;

private:
	UPROPERTY(VisibleAnywhere, Category = "Jungle|Watcher")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Jungle|Watcher")
	TObjectPtr<UStaticMeshComponent> CueMesh;

	UPROPERTY(VisibleAnywhere, Category = "Jungle|Watcher")
	TObjectPtr<UPointLightComponent> CueLight;

	bool bRevealed = false;
};
