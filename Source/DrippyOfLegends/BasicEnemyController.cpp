// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicEnemyController.h"

void ABasicEnemyController::BeginPlay()
{
	Super::BeginPlay();

	AIMoveRequest.SetCanStrafe(true);	
}

void ABasicEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Target == nullptr) return;

	AIMoveRequest.SetGoalActor(Target);

	MoveTo(AIMoveRequest);
}
