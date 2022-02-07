// Copyright Epic Games, Inc. All Rights Reserved.

#include "DrippyOfLegendsCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "AIController.h"

//////////////////////////////////////////////////////////////////////////
// ADrippyOfLegendsCharacter

ADrippyOfLegendsCharacter::ADrippyOfLegendsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1000.0f; // The camera follows at this distance behind the character	
	PreviousTargetZoom = CameraBoom->TargetArmLength;
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	bIsLocked = true;
	bTargetZoomMet = true;
	bDebugMode = false;
	bIsFreeCamera = false;
	bIsAttacking = false;

	ZoomSpeed = 1000.0f;
	MinScrollZoom = 400.0f;
	MaxScrollZoom = 1000.0f;	
	CameraSpeed = 500.0f;
}

void ADrippyOfLegendsCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find world in begin play."));
		return;
	}

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find player controller in begin play."));
		return;
	}

	CameraBoomRelative = CameraBoom->GetRelativeTransform();
	CameraZOffset = CameraBoom->GetComponentLocation().Z;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADrippyOfLegendsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADrippyOfLegendsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADrippyOfLegendsCharacter::MoveRight);

	// Mouse controls
	PlayerInputComponent->BindAction("MouseLeft", EInputEvent::IE_Pressed, this, &ADrippyOfLegendsCharacter::MouseLeft);
	PlayerInputComponent->BindAction("MouseRight", EInputEvent::IE_Pressed, this, &ADrippyOfLegendsCharacter::MouseRight);
	PlayerInputComponent->BindAction("MouseScrollUp", EInputEvent::IE_Pressed, this, &ADrippyOfLegendsCharacter::MouseScrollUp);
	PlayerInputComponent->BindAction("MouseScrollDown", EInputEvent::IE_Pressed, this, &ADrippyOfLegendsCharacter::MouseScrollDown);
	
	//ToggleCameraLock
	PlayerInputComponent->BindAction("ToggleCameraLock", EInputEvent::IE_Pressed, this, &ADrippyOfLegendsCharacter::ToggleCameraLock);

}

void ADrippyOfLegendsCharacter::Tick(float DeltaTime)
{
	if (CameraBoom)
	{
		if (CameraBoom->TargetArmLength < TargetZoom && !bTargetZoomMet)
		{
			CameraBoom->TargetArmLength += ZoomSpeed * DeltaTime;
		}

		if (CameraBoom->TargetArmLength > TargetZoom && !bTargetZoomMet)
		{
			CameraBoom->TargetArmLength -=  ZoomSpeed * DeltaTime;
		}

		if (CameraBoom->TargetArmLength == TargetZoom || CameraBoom->TargetArmLength == TargetZoom + 100 || CameraBoom->TargetArmLength == TargetZoom - 100)
		{
			bTargetZoomMet = true;
		}
	}
}

void ADrippyOfLegendsCharacter::MouseRight()
{
}

void ADrippyOfLegendsCharacter::MouseLeft()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find world in Mouse Left."));
		return;
	}

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find player controller in Mouse Left."));
		return;
	}

	float X, Y;
	if (PlayerController->GetMousePosition(X, Y))
	{
		FVector MouseWorldLocation, MouseWorldDirection;
		
		if (PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
		{
			const float Distance = 99999999.0f;
			FVector Start = FollowCamera->GetComponentLocation();

			// Direction
			FVector DistanceVector = MouseWorldDirection * Distance;
			FVector End = Start + DistanceVector;

			// Collision Params
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);
			FHitResult OutHit;

			// Line Trace
			World->LineTraceSingleByChannel(
				OutHit,
				Start,
				End,
				ECollisionChannel::ECC_Visibility,
				CollisionParams);
			
			// Draw Line
			if (bDebugMode)
				DrawDebugLine(World, Start, End, FColor::Blue, true);

			// Test Actor
			if (OutHit.Actor.IsValid())
			{
				if (bDebugMode)
				{
					UE_LOG(LogTemp, Warning, TEXT("Actor hit : %s"), *OutHit.Actor.Get()->GetName());
					UE_LOG(LogTemp, Warning, TEXT("Location : %s"), *OutHit.Location.ToString());
				}

				FVector TargetLocation = OutHit.Location;

				CancelAttack();

				// Move To Location
				MoveToLocation(TargetLocation);
			}
		}
	}
}

void ADrippyOfLegendsCharacter::MouseScrollUp()
{
	if (CameraBoom)
	{
		TargetZoom = FMath::Clamp((CameraBoom->TargetArmLength - 20.0f), MinScrollZoom, MaxScrollZoom);
		bTargetZoomMet = false;
		
		if(bDebugMode)
			UE_LOG(LogTemp, Warning, TEXT("Target Arm Length : %f"), CameraBoom->TargetArmLength);
	}
}

void ADrippyOfLegendsCharacter::MouseScrollDown()
{
	if (CameraBoom)
	{
		TargetZoom = FMath::Clamp((CameraBoom->TargetArmLength + 20.0f), MinScrollZoom, MaxScrollZoom);
		bTargetZoomMet = false;
		
		if (bDebugMode)
			UE_LOG(LogTemp, Warning, TEXT("Target Arm Length : %f"), CameraBoom->TargetArmLength);
	}
}

void ADrippyOfLegendsCharacter::ToggleCameraLock()
{
	if (bIsLocked) // If true it is unlocked
	{
		// Detach camaera
		CameraBoom->DetachFromComponent(
			FDetachmentTransformRules::FDetachmentTransformRules (
			EDetachmentRule::KeepWorld, 
			EDetachmentRule::KeepRelative, 
			EDetachmentRule::KeepRelative, 
			true
		));

		if (bDebugMode)
			UE_LOG(LogTemp, Warning, TEXT("Detach Camera, Toggle : %b"), bIsLocked);

		bIsFreeCamera = true;

		bIsLocked = false;
	}
	else // If false it is locked
	{
		CameraBoom->SetWorldLocation(GetActorLocation());

		// Reset position
		CameraBoom->SetRelativeTransform(CameraBoomRelative);

		// Attach camera
		CameraBoom->AttachTo(RootComponent, NAME_None, EAttachLocation::KeepRelativeOffset, true);

		if (bDebugMode)
			UE_LOG(LogTemp, Warning, TEXT("Attach Camera, Toggle : %b"), bIsLocked);

		bIsFreeCamera = false;

		bIsLocked = true;
	}
}

void ADrippyOfLegendsCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) 
		&& (Value != 0.0f)
		&& bIsFreeCamera)
	{	
		if (CameraBoom)
		{
			UWorld* World = GetWorld();
			check(World);

			FVector Direction;

			if (Value > 0)
			{
				Direction = FollowCamera->GetUpVector() * ((CameraSpeed) * World->DeltaTimeSeconds);
			}
			else
			{
				Direction = FollowCamera->GetUpVector() * ((-CameraSpeed) * World->DeltaTimeSeconds);
			}

			Direction.Z = 0;

			CameraBoom->AddWorldOffset(Direction);

			UE_LOG(LogTemp, Warning, TEXT("MoveForward: Location : %s"), *CameraBoom->GetComponentLocation().ToString());
		}
	}
}
//FVector Direction = FollowCamera->GetForwardVector() * Value;

void ADrippyOfLegendsCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) 
		&& (Value != 0.0f)
		&& bIsFreeCamera)
	{
		FVector Direction;

		if (CameraBoom)
		{
			UWorld* World = GetWorld();
			check(World);

			if (Value > 0)
			{
				Direction = FollowCamera->GetRightVector() * ((CameraSpeed) * World->DeltaTimeSeconds);
			}
			else
			{
				Direction = FollowCamera->GetRightVector() * ((-CameraSpeed) * World->DeltaTimeSeconds);
			}

			Direction.Z = 0;

			CameraBoom->AddWorldOffset(Direction);

			UE_LOG(LogTemp, Warning, TEXT("MoveRight: Location : %s"), *CameraBoom->GetComponentLocation().ToString());
		}
	}
}
//FVector Direction = FollowCamera->GetRightVector() * Value;
