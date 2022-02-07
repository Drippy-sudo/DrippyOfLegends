// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DrippyOfLegendsCharacter.generated.h"

UCLASS(config=Game)
class ADrippyOfLegendsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	ADrippyOfLegendsCharacter();

	virtual void BeginPlay() override;

protected:
	virtual void Tick(float DeltaTime) override;

	void MouseRight();

	void MouseLeft();

	void MouseScrollUp();

	void MouseScrollDown();

	void ToggleCameraLock();

	UFUNCTION(BlueprintCallable)
	void MoveForward(float Value);

	UFUNCTION(BlueprintCallable)
	void MoveRight(float Value);

	UFUNCTION(BlueprintImplementableEvent)
		void MoveToLocation(FVector Location);

	UFUNCTION(BlueprintImplementableEvent)
		void CancelAttack();

	UPROPERTY(EditAnywhere, Category = "Debug")
		bool bDebugMode;

	UPROPERTY(EditAnywhere, Category = "Camera")
		float ZoomSpeed;

	UPROPERTY(EditAnywhere, Category = "Camera")
		float MinScrollZoom;

	UPROPERTY(EditAnywhere, Category = "Camera")
		float MaxScrollZoom;

	UPROPERTY(EditAnywhere, Category = "Camera")
		float CameraSpeed;

	UPROPERTY(EditAnywhere, Category = "Combat")
		AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bIsAttacking;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	APlayerController* PlayerController;
	float TargetZoom;
	float PreviousTargetZoom;
	bool bTargetZoomMet;
	bool bIsLocked;
	bool bIsFreeCamera;
	FTransform CameraBoomRelative;
	float CameraZOffset;
};

