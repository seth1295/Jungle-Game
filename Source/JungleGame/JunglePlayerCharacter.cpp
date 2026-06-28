#include "JunglePlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HAL/IConsoleManager.h"
#include "JungleGame.h"
#include "JungleInteractableActor.h"

namespace
{
	void JGSetRuntimeViewCVar(const TCHAR* Name, float Value)
	{
		if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
		{
			CVar->Set(Value, ECVF_SetByGameSetting);
		}
	}

	void JGApplyFarViewSettings()
	{
		JGSetRuntimeViewCVar(TEXT("r.ViewDistanceScale"), 20.0f);
		JGSetRuntimeViewCVar(TEXT("foliage.LODDistanceScale"), 20.0f);
		JGSetRuntimeViewCVar(TEXT("r.StaticMeshLODDistanceScale"), 0.1f);
		JGSetRuntimeViewCVar(TEXT("r.SkeletalMeshLODBias"), -10.0f);
		JGSetRuntimeViewCVar(TEXT("r.HLOD.DistanceScale"), 20.0f);
	}
}

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

void AJunglePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocallyControlled() || GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	JGApplyFarViewSettings();
	UE_LOG(LogJungleGame, Display, TEXT("Runtime far-view settings applied on local player client."));
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
