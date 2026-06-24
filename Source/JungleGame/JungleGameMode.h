#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JungleGameMode.generated.h"

UCLASS()
class JUNGLEGAME_API AJungleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AJungleGameMode();

protected:
	virtual void BeginPlay() override;
};
