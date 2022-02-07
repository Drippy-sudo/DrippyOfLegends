// Fill out your copyright notice in the Description page of Project Settings.


#include "DrippyOfLegendsPlayerController.h"

ADrippyOfLegendsPlayerController::ADrippyOfLegendsPlayerController()
{
	// Enable Mouse and Setup
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	DefaultClickTraceChannel = ECollisionChannel::ECC_Visibility;

	//FInputModeGameAndUI InputMode;
	//SetInputMode(InputMode);
}
