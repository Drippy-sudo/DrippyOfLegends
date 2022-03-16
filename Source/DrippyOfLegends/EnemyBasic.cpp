// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBasic.h"
#include "BasicHitbox.h"

// Sets default values
AEnemyBasic::AEnemyBasic()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyBasic::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyBasic::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((Other != this) && Other && OtherComp)
	{
		ABasicHitbox* HitBoxDetected = Cast<ABasicHitbox>(Other);
		if (HitBoxDetected)
		{
			WasHit();
		}
	}
}

// Called every frame
void AEnemyBasic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBasic::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBasic::WasHit_Implementation()
{
	
}

