// Fill out your copyright notice in the Description page of Project Settings.

#include <SlomoPowerup.h>
#include <Engine/World.h>
#include <TimerManager.h>
#include <Kismet/GameplayStatics.h>
#include <VehicleTemplatePawn.h>
#include <EngineUtils.h>


ASlomoPowerup::ASlomoPowerup()
{
	bSlomo = false;
}

void ASlomoPowerup::BeginPlay()
{
	Super::BeginPlay();

	// Setting up initial values for time dilation and post-processing

	OriginalTimeDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());

	TActorIterator<AVehicleTemplatePawn> PlayerPawnIter(GetWorld());
	Camera = (*PlayerPawnIter)->GetCamera();
	if (!Camera->PostProcessSettings.bOverride_SceneFringeIntensity)
	{
		Camera->PostProcessSettings.bOverride_SceneFringeIntensity = true;
	}
	if (!Camera->PostProcessSettings.bOverride_MotionBlurAmount)
	{
		Camera->PostProcessSettings.bOverride_MotionBlurAmount = true;
	}
	OriginalSceneFringeIntensity = Camera->PostProcessSettings.SceneFringeIntensity;
	OriginalMotionBlurAmount = Camera->PostProcessSettings.MotionBlurAmount;
}

void ASlomoPowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ensuring the time dilation and post processing effects are active for the full duration of this powerup
	if (bSlomo)
	{
		if (UGameplayStatics::GetGlobalTimeDilation(GetWorld()) != SlomoValue)
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SlomoValue);
		}
		if (Camera->PostProcessSettings.SceneFringeIntensity != SceneFringeIntensity)
		{
			Camera->PostProcessSettings.SceneFringeIntensity = SceneFringeIntensity;
		}
		if (Camera->PostProcessSettings.MotionBlurAmount != MotionBlurAmount)
		{
			Camera->PostProcessSettings.MotionBlurAmount = MotionBlurAmount;
		}
	}
}

void ASlomoPowerup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);

	// Initiating the slomo effect on overlap
	if (Cast<AVehicleTemplatePawn>(OtherActor) && OtherComponent)
	{
		BeginSlomo();
	}
}

void ASlomoPowerup::BeginSlomo()
{
	// Hiding the powerup from view
	CapsuleCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeshComponent->SetVisibility(false);

	// Setting the 'slomo' boolean to true
	bSlomo = true;

	// Setting a timer for when the slomo effect should end, using SlomoDuration
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASlomoPowerup::EndSlomo, SlomoDuration, false);
}

void ASlomoPowerup::EndSlomo()
{
	// Resetting the time dilation and post-processing effects
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), OriginalTimeDilation);
	Camera->PostProcessSettings.SceneFringeIntensity = OriginalSceneFringeIntensity;
	Camera->PostProcessSettings.MotionBlurAmount = OriginalMotionBlurAmount;

	// Destroying this powerup object
	Destroy();
}
