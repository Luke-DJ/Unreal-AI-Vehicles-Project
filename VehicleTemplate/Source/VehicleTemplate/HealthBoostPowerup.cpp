// Fill out your copyright notice in the Description page of Project Settings.

#include <HealthBoostPowerup.h>
#include <EngineUtils.h>
#include <VehicleTemplatePawn.h>


void AHealthBoostPowerup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);

	// Triggering the health increase of the player on overlap
	if (Cast<AVehicleTemplatePawn>(OtherActor) && OtherComponent)
	{
		AddHealthToPlayer();
		// Destroying this powerup once the health has been added
		Destroy();
	}
}

void AHealthBoostPowerup::AddHealthToPlayer()
{
	// Getting a reference to the player
	TActorIterator<AVehicleTemplatePawn> PlayerPawnIter(GetWorld());
	AVehicleTemplatePawn* Player = *PlayerPawnIter;

	// Increasing the player's health, using HealthIncrease
	Player->IncreaseHealth(HealthIncrease);
}
