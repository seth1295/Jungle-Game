#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JungleFullSizeTerrainShellActor.generated.h"

class UMaterialInterface;
class UProceduralMeshComponent;
class USceneComponent;
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
	static constexpr int32 TerrainVerticesPerSide = 129;
	static constexpr float FullWorldExtentCm = 812800.0f;
	static constexpr float FullWorldExtentMeters = 16256.0f;

	UPROPERTY(VisibleAnywhere, Category = "Terrain Shell")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Terrain Shell")
	TObjectPtr<UProceduralMeshComponent> TerrainMesh;

	UPROPERTY(EditAnywhere, Category = "Terrain Shell|Debug")
	bool bSpawnDebugCubeBlockout = false;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> TerrainMaterial;

	void BuildShell();
	void BuildProceduralTerrainMesh();
	float CalculateTerrainHeightCm(float LocalX, float LocalY) const;
	void LogTerrainMetrics() const;
	void BuildDebugCubeBlockout();
	void AddDebugBlock(const FVector& LocalLocation, const FVector& Scale, FName Name, float LocalYawDegrees = 0.0f);
	void ScheduleTerrainShellSmokeIfRequested();
	void MovePlayerToTerrainShellSmokeView();
	void TakeTerrainShellSmokeShot();
	void ExitAfterTerrainShellSmokeShot();
};
