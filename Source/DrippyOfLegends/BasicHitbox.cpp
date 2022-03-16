// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicHitbox.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"

// Sets default values
ABasicHitbox::ABasicHitbox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(SceneComp);
	HitBox->SetBoxExtent(FVector(50, 50, 50));
	HitBox->bHiddenInGame = false;
}

// Called when the game starts or when spawned
void ABasicHitbox::BeginPlay()
{
	Super::BeginPlay();
	
	//SetLifeSpan(0.1f);
}

// Called every frame
void ABasicHitbox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

