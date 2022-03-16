// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BasicEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class DRIPPYOFLEGENDS_API ABasicEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Target", meta = (AllowPrivateAccess = "true"))
	AActor* Target;

	FAIMoveRequest AIMoveRequest;
};
