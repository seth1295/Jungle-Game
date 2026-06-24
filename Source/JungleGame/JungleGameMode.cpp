#include "JungleGameMode.h"

#include "JungleCell0Director.h"
#include "JungleGame.h"
#include "JunglePlayerCharacter.h"

AJungleGameMode::AJungleGameMode()
{
	DefaultPawnClass = AJunglePlayerCharacter::StaticClass();
}

void AJungleGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		GetWorld()->SpawnActor<AJungleCell0Director>(AJungleCell0Director::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		UE_LOG(LogJungleGame, Display, TEXT("Jungle Cell 0 GameMode started."));
	}
}
