// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Powerup.h>
#include <Camera/CameraComponent.h>
#include "FreezePowerup.generated.h"

/**
 *
 */
UCLASS()
class VEHICLETEMPLATE_API AFreezePowerup : public APowerup
{
	GENERATED_BODY()

public:
	AFreezePowerup();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FreezeDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor SceneColourTint;

private:
	float StandardAngularDamping;
	float StandardLinearDamping;
	float HighAngularDamping;
	float HighLinearDamping;

	FLinearColor OriginalSceneColourTint;

	bool bFrozen;

	UCameraComponent* Camera;

	void FreezeAllEnemies();
	void UnfreezeAllEnemies();
};
