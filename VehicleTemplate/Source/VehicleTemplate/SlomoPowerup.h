// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Powerup.h>
#include <Camera/CameraComponent.h>
#include "SlomoPowerup.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLETEMPLATE_API ASlomoPowerup : public APowerup
{
	GENERATED_BODY()
	
public:
	ASlomoPowerup();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlomoValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlomoDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SceneFringeIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MotionBlurAmount;

private:
	float OriginalTimeDilation;
	float OriginalSceneFringeIntensity;
	float OriginalMotionBlurAmount;

	bool bSlomo;

	UCameraComponent* Camera;

	void BeginSlomo();
	void EndSlomo();
};
