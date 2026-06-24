#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "JunglePlayerCharacter.generated.h"

class UCameraComponent;

UCLASS()
class JUNGLEGAME_API AJunglePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AJunglePlayerCharacter();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void TryInteract();

	UPROPERTY(VisibleAnywhere, Category = "Jungle|Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Jungle|Interaction")
	float InteractionDistance = 350.0f;
};
