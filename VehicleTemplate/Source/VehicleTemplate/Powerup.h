// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include <Components/StaticMeshComponent.h>
#include <Materials/Material.h>
#include <Components/CapsuleComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include "Powerup.generated.h"

UCLASS()
class VEHICLETEMPLATE_API APowerup : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APowerup();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* SpawnInSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* PickupParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator SpinValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BobbingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BobbingStrength;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

private:
	float OriginalZ;
	float LastZ;
};