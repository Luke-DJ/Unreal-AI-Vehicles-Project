// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <BehaviorTree/BTService.h>
#include "BTSteeringService.generated.h"

/**
*
*/
UCLASS()
class VEHICLETEMPLATE_API UBTSteeringService : public UBTService
{
	GENERATED_BODY()

public:
	virtual void OnGameplayTaskActivated(UGameplayTask& task) override;

	virtual void OnGameplayTaskDeactivated(UGameplayTask& task) override;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	class APawn* PlayerPawn;
	
	class AAIWheeledVehicle* AIPawn;
	class AAIWheeledVehicleController* AIController;

	class UBlackboardComponent* BlackboardComp;

	float MinSteering;
	float MaxSteering;
	float MinThrottle;
	float MaxThrottle;

	FVector PlayerLocation;
	FVector EnemyLocation;

	float KPH;

	class UNavigationSystem* NavSys;
	class UNavigationPath* NavPath;

	FVector TargetLocation;

	FVector EnemyForwardVector;
	FVector EnemyRightVector;

	FVector EnemyToTarget;

	float SteeringValue;
	float ThrottleValue;

	//bool IsSetup;

	float ConvertToMinMax(float, float, float);
	float ConvertToMinMax(float, float, float, float, float);

	float AngleFromThreePoints(FVector, FVector, FVector);
};
