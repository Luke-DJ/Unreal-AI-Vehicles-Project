// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/Actor.h>
#include <Components/BoxComponent.h>
#include <AIWheeledVehicle.h>
#include "SpawnPoint.generated.h"

UCLASS()
class VEHICLETEMPLATE_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnPoint();

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere)
	class TSubclassOf<AAIWheeledVehicle> EnemyVehicle;

	UFUNCTION()
	void SpawnEnemyVehicle();
};
