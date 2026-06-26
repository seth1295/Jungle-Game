#include "JungleFullSizeTerrainShellActor.h"

#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "JungleGame.h"
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

	UE_LOG(LogJungleGame, Display, TEXT("Full-size terrain shell spawned: id=JG_FULL_TERRAIN_SHELL_002 extent_m=16256 review_points=8 source=runtime-source-authored-blockout"));
	UE_LOG(LogJungleGame, Display, TEXT("Full-size terrain shell v2 ready: id=JG_FULL_TERRAIN_SHELL_003 grid=%dx%d extent_m=%.0f source=deterministic-procedural-heightfield cube_fallback=%s"), TerrainVerticesPerSide, TerrainVerticesPerSide, FullWorldExtentMeters, bSpawnDebugCubeBlockout ? TEXT("enabled") : TEXT("available"));
	LogTerrainMetrics();
}

void AJungleFullSizeTerrainShellActor::BuildProceduralTerrainMesh()
{
	if (!TerrainMesh)
	{
		return;
	}

	constexpr float HalfExtentCm = FullWorldExtentCm;
	const float StepCm = (HalfExtentCm * 2.0f) / static_cast<float>(TerrainVerticesPerSide - 1);

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	Vertices.Reserve(TerrainVerticesPerSide * TerrainVerticesPerSide);
	Normals.Reserve(TerrainVerticesPerSide * TerrainVerticesPerSide);
	UVs.Reserve(TerrainVerticesPerSide * TerrainVerticesPerSide);
	VertexColors.Reserve(TerrainVerticesPerSide * TerrainVerticesPerSide);
	Tangents.Reserve(TerrainVerticesPerSide * TerrainVerticesPerSide);
	Triangles.Reserve((TerrainVerticesPerSide - 1) * (TerrainVerticesPerSide - 1) * 6);

	for (int32 Y = 0; Y < TerrainVerticesPerSide; ++Y)
	{
		for (int32 X = 0; X < TerrainVerticesPerSide; ++X)
		{
			const float LocalX = -HalfExtentCm + static_cast<float>(X) * StepCm;
			const float LocalY = -HalfExtentCm + static_cast<float>(Y) * StepCm;
			const float HeightCm = CalculateTerrainHeightCm(LocalX, LocalY);

			Vertices.Emplace(LocalX, LocalY, HeightCm);
			Normals.Emplace(FVector::ZeroVector);
			UVs.Emplace(static_cast<float>(X) / static_cast<float>(TerrainVerticesPerSide - 1), static_cast<float>(Y) / static_cast<float>(TerrainVerticesPerSide - 1));
			VertexColors.Emplace(FColor::White);
			Tangents.Emplace(1.0f, 0.0f, 0.0f);
		}
	}

	for (int32 Y = 0; Y < TerrainVerticesPerSide - 1; ++Y)
	{
		for (int32 X = 0; X < TerrainVerticesPerSide - 1; ++X)
		{
			const int32 I0 = X + Y * TerrainVerticesPerSide;
			const int32 I1 = I0 + 1;
			const int32 I2 = I0 + TerrainVerticesPerSide;
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

	TerrainMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	if (TerrainMaterial)
	{
		TerrainMesh->SetMaterial(0, TerrainMaterial);
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
	const FJGTerrainMetrics Metrics = FJungleVolcanicIslandTerrainModel::BuildMetrics(TerrainVerticesPerSide);
	UE_LOG(LogJungleGame, Display, TEXT("Volcanic terrain metrics: %s"), *FJungleVolcanicIslandTerrainModel::BuildMetricsLogLine(Metrics));
}

void AJungleFullSizeTerrainShellActor::BuildDebugCubeBlockout()
{
	AddDebugBlock(FVector(0.0f, 0.0f, -160.0f), FVector(160.0f, 160.0f, 0.18f), TEXT("FullWorldLowlandBasin"));
	AddDebugBlock(FVector(3600.0f, -1800.0f, -130.0f), FVector(110.0f, 24.0f, 0.12f), TEXT("FullWorldCreekValley"), -18.0f);
	AddDebugBlock(FVector(-3200.0f, 2400.0f, 140.0f), FVector(95.0f, 16.0f, 1.0f), TEXT("FullWorldRidgeSpine"), 32.0f);
	AddDebugBlock(FVector(-4100.0f, 3100.0f, 340.0f), FVector(38.0f, 12.0f, 1.8f), TEXT("FullWorldMountainShoulder"), 22.0f);
	AddDebugBlock(FVector(6200.0f, -4800.0f, -175.0f), FVector(70.0f, 32.0f, 0.08f), TEXT("FullWorldCreekMouthCoast"), 10.0f);
	AddDebugBlock(FVector(0.0f, -7200.0f, -220.0f), FVector(210.0f, 16.0f, 0.08f), TEXT("FullWorldSouthCoast"));
	AddDebugBlock(FVector(0.0f, 7600.0f, -260.0f), FVector(220.0f, 14.0f, 0.1f), TEXT("FullWorldNorthOceanEdge"));
	AddDebugBlock(FVector(7200.0f, 0.0f, -230.0f), FVector(14.0f, 220.0f, 0.1f), TEXT("FullWorldEastOceanEdge"));
	AddDebugBlock(FVector(-7200.0f, 0.0f, -230.0f), FVector(14.0f, 220.0f, 0.1f), TEXT("FullWorldWestOceanEdge"));
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

	const FVector ViewLocation(145000.0f, -170000.0f, 18500.0f);
	const FRotator PawnRotation(0.0f, 38.0f, 0.0f);
	const FRotator ViewRotation(-18.0f, 38.0f, 0.0f);
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
