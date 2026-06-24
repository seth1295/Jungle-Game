#include "JunglePlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "JungleGame.h"
#include "JungleInteractableActor.h"

AJunglePlayerCharacter::AJunglePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = 420.0f;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetRootComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
	FirstPersonCamera->bUsePawnControlRotation = true;
}

void AJunglePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AJunglePlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AJunglePlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AJunglePlayerCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AJunglePlayerCharacter::LookUp);
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AJunglePlayerCharacter::TryInteract);
}

void AJunglePlayerCharacter::MoveForward(float Value)
{
	if (!FMath::IsNearlyZero(Value))
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AJunglePlayerCharacter::MoveRight(float Value)
{
	if (!FMath::IsNearlyZero(Value))
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AJunglePlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AJunglePlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AJunglePlayerCharacter::TryInteract()
{
	if (!FirstPersonCamera)
	{
		return;
	}

	const FVector Start = FirstPersonCamera->GetComponentLocation();
	const FVector End = Start + FirstPersonCamera->GetForwardVector() * InteractionDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(JungleInteractTrace), false, this);
	FHitResult Hit;
	if (GetWorld() && GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		if (AJungleInteractableActor* Interactable = Cast<AJungleInteractableActor>(Hit.GetActor()))
		{
			UE_LOG(LogJungleGame, Display, TEXT("Interacted with %s"), *Interactable->GetName());
			Interactable->Interact(this);
		}
	}
}
