// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Powerup.h>
#include "SpeedBoostPowerup.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLETEMPLATE_API ASpeedBoostPowerup : public APowerup
{
	GENERATED_BODY()

public:
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoostStrength;

private:
	void AddForceToPlayer();
};
