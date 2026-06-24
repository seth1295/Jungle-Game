#include "JungleGameMode.h"

#include "JungleCell0Director.h"
#include "JungleCell0LargeWorldIntegrationSpec.h"
#include "JungleGame.h"
#include "JunglePlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

AJungleGameMode::AJungleGameMode()
{
	DefaultPawnClass = AJunglePlayerCharacter::StaticClass();
}

void AJungleGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld())
	{
		return;
	}

	TArray<AActor*> ExistingCell0Directors;
	UGameplayStatics::GetAllActorsOfClass(this, AJungleCell0Director::StaticClass(), ExistingCell0Directors);
	if (ExistingCell0Directors.Num() > 0)
	{
		UE_LOG(LogJungleGame, Display, TEXT("Using placed Jungle Cell 0 director already present in world."));
		return;
	}

	// Fallback source-only placement for PR #19: a real large-world coordinate rather than player-relative spawning.
	// Future map/World Partition work can replace this with an authored placed AJungleCell0Director actor.
	const FVector FallbackCell0WorldOrigin = FJungleCell0LargeWorldIntegrationSpec::FallbackCell0WorldOrigin();
	const FRotator FallbackCell0WorldRotation = FJungleCell0LargeWorldIntegrationSpec::FallbackCell0WorldRotation();
	const FTransform FallbackCell0Transform(FallbackCell0WorldRotation, FallbackCell0WorldOrigin);
	AJungleCell0Director* Cell0Director = GetWorld()->SpawnActorDeferred<AJungleCell0Director>(AJungleCell0Director::StaticClass(), FallbackCell0Transform);
	if (Cell0Director)
	{
		Cell0Director->ConfigureLargeWorldPlacement(FallbackCell0WorldOrigin, FallbackCell0WorldRotation, true);
		Cell0Director->FinishSpawning(FallbackCell0Transform);
		UE_LOG(LogJungleGame, Display, TEXT("Jungle Cell 0 fallback large-world director spawned at %s."), *FallbackCell0WorldOrigin.ToString());
	}
}
