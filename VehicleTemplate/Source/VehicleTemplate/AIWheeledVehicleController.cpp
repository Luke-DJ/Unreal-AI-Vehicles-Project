// Fill out your copyright notice in the Description page of Project Settings.

#include <AIWheeledVehicleController.h>
#include <AIWheeledVehicle.h>
#include <WheeledVehicleMovementComponent4W.h>

AAIWheeledVehicleController::AAIWheeledVehicleController()
{
	VehicleMovementComp = NULL;
	BehaviourTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("AIVehicleBehaviourComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("AIVehicleBlackboardComp"));
}

// Responsible for caching the vehicle pawn's movement component
void AAIWheeledVehicleController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);

	AAIWheeledVehicle* AIVehicle = Cast<AAIWheeledVehicle>(InPawn);

	if (AIVehicle)
	{
		VehicleMovementComp = AIVehicle->GetVehicleMovementComponent();
	}

	if (AIVehicle->BehaviorTree->BlackboardAsset)
	{
		//Cache and initialise the blackboard comp if the BT has one
		BlackboardComp->InitializeBlackboard(*(AIVehicle->BehaviorTree->BlackboardAsset));
	}
	BehaviourTreeComp->StartTree(*AIVehicle->BehaviorTree);
}

void AAIWheeledVehicleController::Tick(float Delta)
{
	Super::Tick(Delta);
}
