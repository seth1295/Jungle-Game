#include "JungleFireActor.h"

#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "JungleGame.h"
#include "UObject/ConstructorHelpers.h"

AJungleFireActor::AJungleFireActor()
{
	PrimaryActorTick.bCanEverTick = true;

	FireMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FireMesh"));
	FireMesh->SetupAttachment(GetRootComponent());
	FireMesh->SetRelativeScale3D(FVector(1.1f, 1.1f, 0.18f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (MeshRef.Succeeded())
	{
		FireMesh->SetStaticMesh(MeshRef.Object);
	}

	FireLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FireLight"));
	FireLight->SetupAttachment(FireMesh);
	FireLight->SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f));
	FireLight->SetAttenuationRadius(650.0f);
}

void AJungleFireActor::BeginPlay()
{
	Super::BeginPlay();
	UpdateVisualState();
}

void AJungleFireActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const float DecayRate = bRaining ? RainDecayPerSecond : DryDecayPerSecond;
	EmberLevel = FMath::Clamp(EmberLevel - DecayRate * DeltaSeconds, 0.0f, 1.0f);
	UpdateVisualState();
}

void AJungleFireActor::Interact(AActor* Interactor)
{
	Super::Interact(Interactor);
	EmberLevel = FMath::Clamp(EmberLevel + TendAmount, 0.0f, 1.0f);
	UpdateVisualState();
	UE_LOG(LogJungleGame, Display, TEXT("Cell 0 fire tended: %.2f"), EmberLevel);
}

FText AJungleFireActor::GetInteractionText() const
{
	return FText::FromString(TEXT("Tend embers"));
}

void AJungleFireActor::SetRaining(bool bNewRaining)
{
	bRaining = bNewRaining;
}

float AJungleFireActor::GetEmberLevel() const
{
	return EmberLevel;
}

void AJungleFireActor::UpdateVisualState()
{
	const float SafeLevel = FMath::Clamp(EmberLevel, 0.0f, 1.0f);
	FireMesh->SetRelativeScale3D(FVector(1.1f, 1.1f, 0.12f + SafeLevel * 0.16f));
	FireLight->SetIntensity(SafeLevel > 0.05f ? 250.0f + SafeLevel * 2600.0f : 0.0f);
}
