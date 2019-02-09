// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include "PhysicsTriggerActor.generated.h"

UCLASS()
class VEHICLETEMPLATE_API APhysicsTriggerActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent *MeshComponent;
public:
	// Sets default values for this actor's properties
	APhysicsTriggerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//The delegate function for handling an overlap event
	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	//The delegate function for handling a hit event
	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
};
