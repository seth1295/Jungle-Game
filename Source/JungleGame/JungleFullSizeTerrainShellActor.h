#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JungleFullSizeTerrainShellActor.generated.h"

class UMaterialInterface;
class UProceduralMeshComponent;
class USceneComponent;
class UStaticMesh;
struct FJGTerrainRuntimeTileDesc;

UCLASS()
class JUNGLEGAME_API AJungleFullSizeTerrainShellActor : public AActor
{
	GENERATED_BODY()

public:
	AJungleFullSizeTerrainShellActor();

protected:
	virtual void BeginPlay() override;

private:
	static constexpr float FullWorldExtentCm = 812800.0f;
	static constexpr float FullWorldExtentMeters = 16256.0f;

	UPROPERTY(VisibleAnywhere, Category = "Terrain Shell")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Terrain Shell")
	TObjectPtr<UProceduralMeshComponent> TerrainMesh;

	UPROPERTY(EditAnywhere, Category = "Terrain Shell|Debug")
	bool bSpawnDebugCubeBlockout = false;

	UPROPERTY(EditAnywhere, Category = "Terrain Shell|Runtime")
	int32 RuntimeValidationTilesPerSide = 5;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> TerrainMaterial;

	void BuildShell();
	void BuildProceduralTerrainMesh();
	void BuildRuntimeTileMeshSection(int32 SectionIndex, const FJGTerrainRuntimeTileDesc& TileDesc);
	float CalculateTerrainHeightCm(float LocalX, float LocalY) const;
	void LogTerrainMetrics() const;
	void LogRuntimeMeshMetrics(const TArray<FJGTerrainRuntimeTileDesc>& TileDescs) const;
	void BuildDebugCubeBlockout();
	void AddDebugBlock(const FVector& LocalLocation, const FVector& Scale, FName Name, float LocalYawDegrees = 0.0f);
	void ScheduleTerrainShellSmokeIfRequested();
	void MovePlayerToTerrainShellSmokeView();
	void TakeTerrainShellSmokeShot();
	void ExitAfterTerrainShellSmokeShot();
};
