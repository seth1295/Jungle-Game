#include "JungleFullSizeTerrainShellActor.h"

#include "Engine/StaticMeshActor.h"
#include "JungleGame.h"
#include "UObject/ConstructorHelpers.h"

AJungleFullSizeTerrainShellActor::AJungleFullSizeTerrainShellActor()
{
	PrimaryActorTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshRef.Succeeded())
	{
		CubeMesh = MeshRef.Object;
	}
}

void AJungleFullSizeTerrainShellActor::BeginPlay()
{
	Super::BeginPlay();
	BuildShell();
}

void AJungleFullSizeTerrainShellActor::BuildShell()
{
	UE_LOG(LogJungleGame, Display, TEXT("Full-size terrain shell spawned: id=JG_FULL_TERRAIN_SHELL_002 extent_m=16256 review_points=8 source=runtime-source-authored-blockout"));

	AddBlock(FVector(0.0f, 0.0f, -160.0f), FVector(160.0f, 160.0f, 0.18f), TEXT("FullWorldLowlandBasin"));
	AddBlock(FVector(3600.0f, -1800.0f, -130.0f), FVector(110.0f, 24.0f, 0.12f), TEXT("FullWorldCreekValley"), -18.0f);
	AddBlock(FVector(-3200.0f, 2400.0f, 140.0f), FVector(95.0f, 16.0f, 1.0f), TEXT("FullWorldRidgeSpine"), 32.0f);
	AddBlock(FVector(-4100.0f, 3100.0f, 340.0f), FVector(38.0f, 12.0f, 1.8f), TEXT("FullWorldMountainShoulder"), 22.0f);
	AddBlock(FVector(6200.0f, -4800.0f, -175.0f), FVector(70.0f, 32.0f, 0.08f), TEXT("FullWorldCreekMouthCoast"), 10.0f);
	AddBlock(FVector(0.0f, -7200.0f, -220.0f), FVector(210.0f, 16.0f, 0.08f), TEXT("FullWorldSouthCoast"));
	AddBlock(FVector(0.0f, 7600.0f, -260.0f), FVector(220.0f, 14.0f, 0.1f), TEXT("FullWorldNorthOceanEdge"));
	AddBlock(FVector(7200.0f, 0.0f, -230.0f), FVector(14.0f, 220.0f, 0.1f), TEXT("FullWorldEastOceanEdge"));
	AddBlock(FVector(-7200.0f, 0.0f, -230.0f), FVector(14.0f, 220.0f, 0.1f), TEXT("FullWorldWestOceanEdge"));
}

void AJungleFullSizeTerrainShellActor::AddBlock(const FVector& LocalLocation, const FVector& Scale, FName Name, float LocalYawDegrees)
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
