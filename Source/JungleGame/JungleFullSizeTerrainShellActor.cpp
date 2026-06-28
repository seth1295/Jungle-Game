#include "JungleFullSizeTerrainShellActor.h"

#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "JungleGame.h"
#include "JungleTerrainBatch006Calibration.h"
#include "JungleVolcanicIslandTerrainModel.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "ProceduralMeshComponent.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AJungleFullSizeTerrainShellActor::AJungleFullSizeTerrainShellActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("FullSizeProceduralTerrain"));
	TerrainMesh->SetupAttachment(SceneRoot);
	TerrainMesh->bUseAsyncCooking = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshRef.Succeeded())
	{
		CubeMesh = MeshRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialRef(TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
	if (MaterialRef.Succeeded())
	{
		TerrainMaterial = MaterialRef.Object;
	}
}

void AJungleFullSizeTerrainShellActor::BeginPlay()
{
	Super::BeginPlay();
	BuildShell();
	ScheduleTerrainShellSmokeIfRequested();
}

void AJungleFullSizeTerrainShellActor::BuildShell()
{
	BuildProceduralTerrainMesh();

	if (bSpawnDebugCubeBlockout || FParse::Param(FCommandLine::Get(), TEXT("JungleDebugCubeShell")))
	{
		BuildDebugCubeBlockout();
	}

	UE_LOG(LogJungleGame, Display, TEXT("Full-size terrain shell spawned: id=JG_FULL_TERRAIN_SHELL_004_X6 extent_m=%.0f review_points=8 source=runtime-source-authored-x6-terrain"), FullWorldExtentMeters);
	UE_LOG(LogJungleGame, Display, TEXT("Full-size terrain shell v4 ready: id=JG_FULL_TERRAIN_SHELL_004_X6_ANTI_RADIAL grid=%dx%d extent_m=%.0f source=deterministic-procedural-heightfield cube_fallback=%s"), FJungleVolcanicIslandTerrainModel::RuntimePreviewVerticesPerSide, FJungleVolcanicIslandTerrainModel::RuntimePreviewVerticesPerSide, FullWorldExtentMeters, bSpawnDebugCubeBlockout ? TEXT("enabled") : TEXT("available"));
	LogTerrainMetrics();
}

void AJungleFullSizeTerrainShellActor::BuildProceduralTerrainMesh()
{
	if (!TerrainMesh)
	{
		return;
	}

	TerrainMesh->ClearAllMeshSections();

	TArray<FJGTerrainRuntimeTileDesc> TileDescs;
	FJungleVolcanicIslandTerrainModel::BuildRuntimeValidationTileDescs(TileDescs);

	for (int32 TileIndex = 0; TileIndex < TileDescs.Num(); ++TileIndex)
	{
		BuildRuntimeTileMeshSection(TileIndex, TileDescs[TileIndex]);
	}

	LogRuntimeMeshMetrics(TileDescs);
	const FJGTerrainChannelMetrics ChannelMetrics = FJungleVolcanicIslandTerrainModel::BuildChannelMetrics(FJungleVolcanicIslandTerrainModel::RuntimePreviewVerticesPerSide);
	UE_LOG(LogJungleGame, Display, TEXT("Terrain channel metrics: %s"), *FJungleVolcanicIslandTerrainModel::BuildChannelMetricsLogLine(ChannelMetrics));
	const FJGTerrainTopographicMetrics TopographicMetrics = FJungleVolcanicIslandTerrainModel::BuildTopographicMetrics(FJungleVolcanicIslandTerrainModel::RuntimePreviewVerticesPerSide);
	UE_LOG(LogJungleGame, Display, TEXT("Terrain topographic export metrics: %s"), *FJungleVolcanicIslandTerrainModel::BuildTopographicMetricsLogLine(TopographicMetrics));
	const FJGTerrainGeneratorConfig GeneratorConfig = FJungleVolcanicIslandTerrainModel::BuildDefaultGeneratorConfig();
	const FJGTerrainGeneratorArchitectureMetrics ArchitectureMetrics = FJungleVolcanicIslandTerrainModel::BuildGeneratorArchitectureMetrics(GeneratorConfig);
	UE_LOG(LogJungleGame, Display, TEXT("Terrain generator architecture metrics: %s"), *FJungleVolcanicIslandTerrainModel::BuildGeneratorArchitectureLogLine(ArchitectureMetrics));
	const FJGTerrainBatchAcceptanceMetrics BatchAcceptanceMetrics = FJungleVolcanicIslandTerrainModel::BuildBatchAcceptanceMetrics();
	UE_LOG(LogJungleGame, Display, TEXT("Terrain batch acceptance metrics: %s"), *FJungleVolcanicIslandTerrainModel::BuildBatchAcceptanceLogLine(BatchAcceptanceMetrics));
	const FJGBatch006DemBenchmarkMetrics DemBenchmarkMetrics = FJungleTerrainBatch006Calibration::BuildDemBenchmarkMetrics(FJungleVolcanicIslandTerrainModel::RuntimePreviewVerticesPerSide);
	UE_LOG(LogJungleGame, Display, TEXT("Batch006 DEM benchmark metrics: %s"), *FJungleTerrainBatch006Calibration::BuildDemBenchmarkLogLine(DemBenchmarkMetrics));
	const FJGBatch006OfflineBridgeMetrics OfflineBridgeMetrics = FJungleTerrainBatch006Calibration::BuildOfflineBridgeMetrics();
	UE_LOG(LogJungleGame, Display, TEXT("Batch006 offline/runtime bridge metrics: %s"), *FJungleTerrainBatch006Calibration::BuildOfflineBridgeLogLine(OfflineBridgeMetrics));
	const FJGBatch006PreviewValidationMetrics PreviewValidationMetrics = FJungleTerrainBatch006Calibration::BuildPreviewValidationMetrics(FJungleVolcanicIslandTerrainModel::RuntimePreviewVerticesPerSide);
	UE_LOG(LogJungleGame, Display, TEXT("Batch006 morphology preview metrics: %s"), *FJungleTerrainBatch006Calibration::BuildPreviewValidationLogLine(PreviewValidationMetrics));
	const FJGBatch006AcceptanceMetrics Batch006AcceptanceMetrics = FJungleTerrainBatch006Calibration::BuildAcceptanceMetrics();
	UE_LOG(LogJungleGame, Display, TEXT("Batch006 terrain acceptance metrics: %s"), *FJungleTerrainBatch006Calibration::BuildAcceptanceLogLine(Batch006AcceptanceMetrics));
}

void AJungleFullSizeTerrainShellActor::BuildRuntimeTileMeshSection(int32 SectionIndex, const FJGTerrainRuntimeTileDesc& TileDesc)
{
	if (!TerrainMesh || TileDesc.VerticesPerSide < 2)
	{
		return;
	}

	const int32 VerticesPerSide = TileDesc.VerticesPerSide;
	const int32 VertexCount = VerticesPerSide * VerticesPerSide;
	const int32 QuadCountPerSide = VerticesPerSide - 1;

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	Vertices.Reserve(VertexCount);
	Normals.Reserve(VertexCount);
	UVs.Reserve(VertexCount);
	VertexColors.Reserve(VertexCount);
	Tangents.Reserve(VertexCount);
	Triangles.Reserve(QuadCountPerSide * QuadCountPerSide * 6);

	for (int32 Y = 0; Y < VerticesPerSide; ++Y)
	{
		const float AlphaY = static_cast<float>(Y) / static_cast<float>(VerticesPerSide - 1);
		const float WorldYM = FMath::Lerp(TileDesc.MinYM, TileDesc.MaxYM, AlphaY);
		for (int32 X = 0; X < VerticesPerSide; ++X)
		{
			const float AlphaX = static_cast<float>(X) / static_cast<float>(VerticesPerSide - 1);
			const float WorldXM = FMath::Lerp(TileDesc.MinXM, TileDesc.MaxXM, AlphaX);
			const FJGTerrainSample TerrainSample = FJungleVolcanicIslandTerrainModel::SampleTerrainMeters(WorldXM, WorldYM);

			Vertices.Emplace(WorldXM * 100.0f, WorldYM * 100.0f, TerrainSample.HeightM * 100.0f);
			Normals.Emplace(FVector::ZeroVector);
			UVs.Emplace((WorldXM + FJungleVolcanicIslandTerrainModel::HalfExtentM) / FJungleVolcanicIslandTerrainModel::WorldSizeM, (WorldYM + FJungleVolcanicIslandTerrainModel::HalfExtentM) / FJungleVolcanicIslandTerrainModel::WorldSizeM);
			const uint8 CoastDebug = static_cast<uint8>(FMath::Clamp(TerrainSample.BeachRingMask * 255.0f, 0.0f, 255.0f));
			const uint8 RidgeDebug = static_cast<uint8>(FMath::Clamp(TerrainSample.RidgeMask * 255.0f, 0.0f, 255.0f));
			const uint8 HazardDebug = static_cast<uint8>(FMath::Clamp(TerrainSample.HardBlockerMask * 255.0f, 0.0f, 255.0f));
			VertexColors.Emplace(CoastDebug, RidgeDebug, HazardDebug, 255);
			Tangents.Emplace(1.0f, 0.0f, 0.0f);
		}
	}

	for (int32 Y = 0; Y < QuadCountPerSide; ++Y)
	{
		for (int32 X = 0; X < QuadCountPerSide; ++X)
		{
			const int32 I0 = X + Y * VerticesPerSide;
			const int32 I1 = I0 + 1;
			const int32 I2 = I0 + VerticesPerSide;
			const int32 I3 = I2 + 1;

			Triangles.Add(I0);
			Triangles.Add(I2);
			Triangles.Add(I1);
			Triangles.Add(I1);
			Triangles.Add(I2);
			Triangles.Add(I3);
		}
	}

	for (int32 TriangleIndex = 0; TriangleIndex < Triangles.Num(); TriangleIndex += 3)
	{
		const int32 A = Triangles[TriangleIndex];
		const int32 B = Triangles[TriangleIndex + 1];
		const int32 C = Triangles[TriangleIndex + 2];
		const FVector FaceNormal = FVector::CrossProduct(Vertices[B] - Vertices[A], Vertices[C] - Vertices[A]).GetSafeNormal();
		Normals[A] += FaceNormal;
		Normals[B] += FaceNormal;
		Normals[C] += FaceNormal;
	}

	for (FVector& Normal : Normals)
	{
		Normal = Normal.IsNearlyZero() ? FVector::UpVector : Normal.GetSafeNormal();
	}

	TerrainMesh->CreateMeshSection(SectionIndex, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, TileDesc.bCollisionEnabled);
	if (TerrainMaterial)
	{
		TerrainMesh->SetMaterial(SectionIndex, TerrainMaterial);
	}
}

float AJungleFullSizeTerrainShellActor::CalculateTerrainHeightCm(float LocalX, float LocalY) const
{
	const float WorldXM = LocalX * 0.01f;
	const float WorldYM = LocalY * 0.01f;
	return FJungleVolcanicIslandTerrainModel::SampleHeightMeters(WorldXM, WorldYM) * 100.0f;
}

void AJungleFullSizeTerrainShellActor::LogTerrainMetrics() const
{
	const FJGTerrainMetrics Metrics = FJungleVolcanicIslandTerrainModel::BuildMetrics(FJungleVolcanicIslandTerrainModel::RuntimePreviewVerticesPerSide);
	UE_LOG(LogJungleGame, Display, TEXT("Volcanic terrain metrics: %s"), *FJungleVolcanicIslandTerrainModel::BuildMetricsLogLine(Metrics));
}

void AJungleFullSizeTerrainShellActor::LogRuntimeMeshMetrics(const TArray<FJGTerrainRuntimeTileDesc>& TileDescs) const
{
	const FJGTerrainRuntimeMeshMetrics RuntimeMetrics = FJungleVolcanicIslandTerrainModel::BuildRuntimeMeshMetrics(TileDescs);
	UE_LOG(LogJungleGame, Display, TEXT("Runtime mesh metrics: %s"), *FJungleVolcanicIslandTerrainModel::BuildRuntimeMeshMetricsLogLine(RuntimeMetrics));
}

void AJungleFullSizeTerrainShellActor::BuildDebugCubeBlockout()
{
	AddDebugBlock(FVector(0.0f, 0.0f, -220.0f), FVector(960.0f, 960.0f, 0.18f), TEXT("FullWorldLowlandBasin_X6"));
	AddDebugBlock(FVector(21600.0f, -10800.0f, -180.0f), FVector(660.0f, 144.0f, 0.12f), TEXT("FullWorldCreekValley_X6"), -18.0f);
	AddDebugBlock(FVector(-19200.0f, 14400.0f, 520.0f), FVector(570.0f, 96.0f, 1.4f), TEXT("FullWorldIrregularRidgeSpine_X6"), 32.0f);
	AddDebugBlock(FVector(-24600.0f, 18600.0f, 1280.0f), FVector(228.0f, 72.0f, 2.4f), TEXT("FullWorldMountainShoulder_X6"), 22.0f);
	AddDebugBlock(FVector(37200.0f, -28800.0f, -190.0f), FVector(420.0f, 192.0f, 0.08f), TEXT("FullWorldCreekMouthCoast_X6"), 10.0f);
	AddDebugBlock(FVector(0.0f, -43200.0f, -240.0f), FVector(1260.0f, 96.0f, 0.08f), TEXT("FullWorldSouthCoast_X6"));
	AddDebugBlock(FVector(0.0f, 45600.0f, -280.0f), FVector(1320.0f, 84.0f, 0.1f), TEXT("FullWorldNorthOceanEdge_X6"));
	AddDebugBlock(FVector(45600.0f, 0.0f, -250.0f), FVector(84.0f, 1320.0f, 0.1f), TEXT("FullWorldEastOceanEdge_X6"));
	AddDebugBlock(FVector(-45600.0f, 0.0f, -250.0f), FVector(84.0f, 1320.0f, 0.1f), TEXT("FullWorldWestOceanEdge_X6"));
}

void AJungleFullSizeTerrainShellActor::AddDebugBlock(const FVector& LocalLocation, const FVector& Scale, FName Name, float LocalYawDegrees)
{
	if (!CubeMesh || !GetWorld())
	{
		return;
	}

	AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), LocalLocation, FRotator(0.0f, LocalYawDegrees, 0.0f));
	if (!MeshActor)
	{
		return;
	}

#if WITH_EDITOR
	MeshActor->SetActorLabel(Name.ToString());
#endif
	MeshActor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
	MeshActor->SetActorScale3D(Scale);
}

void AJungleFullSizeTerrainShellActor::ScheduleTerrainShellSmokeIfRequested()
{
	if (!FParse::Param(FCommandLine::Get(), TEXT("JungleTerrainShellSmoke")))
	{
		return;
	}

	FTimerHandle ViewTimer;
	GetWorldTimerManager().SetTimer(ViewTimer, this, &AJungleFullSizeTerrainShellActor::MovePlayerToTerrainShellSmokeView, 0.75f, false);
	FTimerHandle ShotTimer;
	GetWorldTimerManager().SetTimer(ShotTimer, this, &AJungleFullSizeTerrainShellActor::TakeTerrainShellSmokeShot, 3.0f, false);
	UE_LOG(LogJungleGame, Display, TEXT("Full-size terrain shell smoke capture scheduled."));
}

void AJungleFullSizeTerrainShellActor::MovePlayerToTerrainShellSmokeView()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerPawn || !PlayerController)
	{
		UE_LOG(LogJungleGame, Warning, TEXT("Full-size terrain shell smoke view skipped: missing player controller or pawn."));
		return;
	}

	const FVector ViewLocation(870000.0f, -1020000.0f, 62000.0f);
	const FRotator PawnRotation(0.0f, 38.0f, 0.0f);
	const FRotator ViewRotation(-12.0f, 38.0f, 0.0f);
	PlayerPawn->SetActorLocationAndRotation(ViewLocation, PawnRotation, false, nullptr, ETeleportType::TeleportPhysics);
	PlayerController->SetControlRotation(ViewRotation);
	UE_LOG(LogJungleGame, Display, TEXT("Full-size terrain shell smoke view set at %s with rotation %s."), *ViewLocation.ToString(), *ViewRotation.ToString());
}

void AJungleFullSizeTerrainShellActor::TakeTerrainShellSmokeShot()
{
	if (!GEngine || !GetWorld())
	{
		UE_LOG(LogJungleGame, Warning, TEXT("Full-size terrain shell smoke shot skipped: engine or world missing."));
		return;
	}

	UE_LOG(LogJungleGame, Display, TEXT("Full-size terrain shell smoke shot requested."));
	GEngine->Exec(GetWorld(), TEXT("HighResShot 1920x1080"));
	FTimerHandle ExitTimer;
	GetWorldTimerManager().SetTimer(ExitTimer, this, &AJungleFullSizeTerrainShellActor::ExitAfterTerrainShellSmokeShot, 1.5f, false);
}

void AJungleFullSizeTerrainShellActor::ExitAfterTerrainShellSmokeShot()
{
	if (GEngine && GetWorld())
	{
		GEngine->Exec(GetWorld(), TEXT("quit"));
	}
}
