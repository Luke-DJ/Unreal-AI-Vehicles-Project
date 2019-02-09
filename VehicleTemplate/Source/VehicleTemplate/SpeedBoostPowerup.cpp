// Fill out your copyright notice in the Description page of Project Settings.

#include <SpeedBoostPowerup.h>
#include <EngineUtils.h>
#include <VehicleTemplatePawn.h>
#include <Components/SkeletalMeshComponent.h>


void ASpeedBoostPowerup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);

	// Triggering the speed increase to the player on overlap
	if (Cast<AVehicleTemplatePawn>(OtherActor) && OtherComponent)
	{
		AddForceToPlayer();
		Destroy();
	}
}

void ASpeedBoostPowerup::AddForceToPlayer()
{
	// Getting a reference to the player's mesh
	TActorIterator<AVehicleTemplatePawn> PlayerPawnIter(GetWorld());
	AVehicleTemplatePawn* Player = *PlayerPawnIter;
	USkeletalMeshComponent* PlayerMesh = Player->GetMesh();

	// Applying a force to the player's mesh, using BoostStrength
	PlayerMesh->AddForce(Player->GetActorForwardVector() * PlayerMesh->GetBodyInstance()->GetBodyMass() * BoostStrength);
}
