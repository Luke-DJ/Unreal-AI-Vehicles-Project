// Fill out your copyright notice in the Description page of Project Settings.

#include <FreezePowerup.h>
#include <Engine/World.h>
#include <TimerManager.h>
#include <Kismet/GameplayStatics.h>
#include <VehicleTemplatePawn.h>
#include <EngineUtils.h>
#include <AIWheeledVehicle.h>
#include <Components/SkeletalMeshComponent.h>


AFreezePowerup::AFreezePowerup()
{
	// Values for the original and 'high' dampening values
	StandardAngularDamping = 0.01f;
	StandardLinearDamping = 0.0f;
	HighAngularDamping = 1000000.0f;
	HighLinearDamping = 1000000.0f;

	// Boolean representing the 'frozen' state of the AIs
	bFrozen = false;
}

void AFreezePowerup::BeginPlay()
{
	Super::BeginPlay();

	// Getting references to the player and the player's camera
	TActorIterator<AVehicleTemplatePawn> PlayerPawnIter(GetWorld());
	Camera = (*PlayerPawnIter)->GetCamera();
	if (!Camera->PostProcessSettings.bOverride_SceneColorTint)
	{
		Camera->PostProcessSettings.bOverride_SceneColorTint = true;
	}
	OriginalSceneColourTint = Camera->PostProcessSettings.SceneColorTint;
}

void AFreezePowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ensuring the AIs are frozen, and that post processing effect is active, for the full duration of this powerup
	if (bFrozen)
	{
		USkeletalMeshComponent* MeshComponent;
		for (TActorIterator<AAIWheeledVehicle> EnemyItr(GetWorld()); EnemyItr; ++EnemyItr)
		{
			MeshComponent = (*EnemyItr)->GetMesh();
			if (MeshComponent->GetAngularDamping() != HighAngularDamping)
			{
				MeshComponent->SetAngularDamping(HighAngularDamping);
			}
			if (MeshComponent->GetLinearDamping() != HighLinearDamping)
			{
				MeshComponent->SetLinearDamping(HighLinearDamping);
			}
		}
		if (Camera->PostProcessSettings.SceneColorTint != SceneColourTint)
		{
			Camera->PostProcessSettings.SceneColorTint = SceneColourTint;
		}
	}
}

void AFreezePowerup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);

	// Triggering the 'freezing' of all enemies on overlap
	if (Cast<AVehicleTemplatePawn>(OtherActor) && OtherComponent)
	{
		FreezeAllEnemies();
	}
}

void AFreezePowerup::FreezeAllEnemies()
{
	// Hiding the powerup from view
	CapsuleCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeshComponent->SetVisibility(false);

	// Setting the 'frozen' boolean to true
	bFrozen = true;

	// Setting a timer for when the enemies should be 'unfrozen', using FreezeDuration
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AFreezePowerup::UnfreezeAllEnemies, FreezeDuration, false);
}

void AFreezePowerup::UnfreezeAllEnemies()
{
	USkeletalMeshComponent* MeshComponent;
	// Iterating over all enemies, and setting their modified values back to defaults
	for (TActorIterator<AAIWheeledVehicle> EnemyItr(GetWorld()); EnemyItr; ++EnemyItr)
	{
		MeshComponent = (*EnemyItr)->GetMesh();
		MeshComponent->SetAngularDamping(StandardAngularDamping);
		MeshComponent->SetLinearDamping(StandardLinearDamping);
	}
	// Resetting the post-processing effect
	Camera->PostProcessSettings.SceneColorTint = OriginalSceneColourTint;

	// Destroying this powerup object
	Destroy();
}
