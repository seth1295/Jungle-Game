#include "JungleCell0Director.h"

#include "Engine/StaticMeshActor.h"
#include "JungleCrossingActor.h"
#include "JungleFireActor.h"
#include "JungleGame.h"
#include "JungleMarkerActor.h"
#include "JungleWatcherCueActor.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AJungleCell0Director::AJungleCell0Director()
{
	PrimaryActorTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshRef.Succeeded())
	{
		CubeMesh = MeshRef.Object;
	}
}

void AJungleCell0Director::BeginPlay()
{
	Super::BeginPlay();
	BuildCell();

	GetWorldTimerManager().SetTimerForNextTick(this, &AJungleCell0Director::StartRain);
	FTimerHandle CrossingTimer;
	GetWorldTimerManager().SetTimer(CrossingTimer, this, &AJungleCell0Director::ChangeCrossing, 8.0f, false);
	FTimerHandle CueTimer;
	GetWorldTimerManager().SetTimer(CueTimer, this, &AJungleCell0Director::ShowCue, 14.0f, false);
}

void AJungleCell0Director::BuildCell()
{
	FireActor = GetWorld()->SpawnActor<AJungleFireActor>(AJungleFireActor::StaticClass(), FVector(180.0f, 0.0f, 40.0f), FRotator::ZeroRotator);
	CrossingActor = GetWorld()->SpawnActor<AJungleCrossingActor>(AJungleCrossingActor::StaticClass(), FVector(980.0f, 0.0f, 70.0f), FRotator::ZeroRotator);
	MarkerActor = GetWorld()->SpawnActor<AJungleMarkerActor>(AJungleMarkerActor::StaticClass(), FVector(1080.0f, 170.0f, 55.0f), FRotator(0.0f, 25.0f, 0.0f));
	CueActor = GetWorld()->SpawnActor<AJungleWatcherCueActor>(AJungleWatcherCueActor::StaticClass(), FVector(1500.0f, -520.0f, 220.0f), FRotator::ZeroRotator);

	AddCube(FVector(350.0f, 0.0f, -25.0f), FVector(9.0f, 5.0f, 0.1f), TEXT("CampGround"));
	AddCube(FVector(700.0f, -260.0f, 90.0f), FVector(0.45f, 0.45f, 2.4f), TEXT("DenseTrunkA"));
	AddCube(FVector(740.0f, 260.0f, 90.0f), FVector(0.45f, 0.45f, 2.4f), TEXT("DenseTrunkB"));
	AddCube(FVector(1110.0f, 0.0f, -10.0f), FVector(5.0f, 1.6f, 0.08f), TEXT("CreekBed"));

	UE_LOG(LogJungleGame, Display, TEXT("Jungle Cell 0 runtime blockout spawned."));
}

void AJungleCell0Director::StartRain()
{
	if (FireActor)
	{
		FireActor->SetRaining(true);
	}
	UE_LOG(LogJungleGame, Display, TEXT("Rain pressure started."));
}

void AJungleCell0Director::ChangeCrossing()
{
	if (CrossingActor)
	{
		CrossingActor->ApplyStateChange();
	}
	if (MarkerActor)
	{
		MarkerActor->RevealMarker();
	}
}

void AJungleCell0Director::ShowCue()
{
	if (CueActor)
	{
		CueActor->RevealCue();
	}
}

void AJungleCell0Director::AddCube(const FVector& Location, const FVector& Scale, const FName Name)
{
	AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, FRotator::ZeroRotator);
	if (!MeshActor || !CubeMesh)
	{
		return;
	}

	MeshActor->SetActorLabel(Name.ToString());
	MeshActor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
	MeshActor->SetActorScale3D(Scale);
}
