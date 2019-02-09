// Fill out your copyright notice in the Description page of Project Settings.

#include <BTTaskSteerVehicle.h>
#include <BehaviorTree/BTTaskNode.h>
#include <BehaviorTree/BehaviorTreeTypes.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <AIWheeledVehicleController.h>
#include <WheeledVehicleMovementComponent4W.h>


EBTNodeResult::Type UBTTaskSteerVehicle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIWheeledVehicleController* AIController = Cast<AAIWheeledVehicleController>(OwnerComp.GetAIOwner());

	if (AIController)
	{
		// Getting references to the AI controller and the blackboard
		AAIWheeledVehicleController* AIController = Cast<AAIWheeledVehicleController>(OwnerComp.GetAIOwner());
		//UBlackboardComponent* BlackboardComp = AIController->BlackboardComp;
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

		// Setting the steering of the AI vehicle to the value stored in the blackboard
		AIController->VehicleMovementComp->SetSteeringInput(BlackboardComp->GetValueAsFloat("SteeringValue"));

		//inform BT this node has successfully completed its task
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UBTTaskSteerVehicle::OnGameplayTaskActivated(UGameplayTask & task)
{
	//NB: this method must be overridden even if it has an empty body
	//otherwise the linker will fail
}
