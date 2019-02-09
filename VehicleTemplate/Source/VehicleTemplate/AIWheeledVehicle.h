// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <WheeledVehicle.h>
#include <Particles/ParticleSystemComponent.h>
#include <ParticleDefinitions.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include "AIWheeledVehicle.generated.h"

/**
*
*/
UCLASS()
class AAIWheeledVehicle : public AWheeledVehicle
{
	GENERATED_BODY()

public:
	AAIWheeledVehicle();

	virtual void PostInitializeComponents() override;
	virtual void Tick(float Delta) override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComponent;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere)
	float MinSteering = 0.0f;

	UPROPERTY(EditAnywhere)
	float MaxSteering;

	UPROPERTY(EditAnywhere)
	float MinThrottle;

	UPROPERTY(EditAnywhere)
	float MaxThrottle;

	UPROPERTY(EditAnywhere)
	int DamageToPlayer;

	UPROPERTY(EditAnywhere)
	USoundCue* ExplosionSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ExplosionParticleSystem;

	void Kill();

protected:
	/* Are we on a 'slippery' surface */
	bool bIsLowFriction;
};
