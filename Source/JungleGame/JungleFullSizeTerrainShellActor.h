#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JungleFullSizeTerrainShellActor.generated.h"

class UStaticMesh;

UCLASS()
class JUNGLEGAME_API AJungleFullSizeTerrainShellActor : public AActor
{
	GENERATED_BODY()

public:
	AJungleFullSizeTerrainShellActor();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	void BuildShell();
	void AddBlock(const FVector& LocalLocation, const FVector& Scale, FName Name, float LocalYawDegrees = 0.0f);
};
