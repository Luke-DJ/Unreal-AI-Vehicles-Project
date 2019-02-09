// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnPoint.h"


ASpawnPoint::ASpawnPoint()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Root Component"));
	BoxComponent->SetBoxExtent(FVector(0.0f, 0.0f, 0.0f));
	BoxComponent->SetHiddenInGame(true);
	RootComponent = BoxComponent;
}

void ASpawnPoint::SpawnEnemyVehicle()
{
	AAIWheeledVehicle* EnemyVehicleInstance = GetWorld()->SpawnActor<AAIWheeledVehicle>(EnemyVehicle, GetTransform());
	EnemyVehicleInstance->SpawnDefaultController();
}
