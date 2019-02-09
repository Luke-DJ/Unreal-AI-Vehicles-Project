// Fill out your copyright notice in the Description page of Project Settings.

#include <BTTaskThrottle.h>
#include <BehaviorTree/BTTaskNode.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/BehaviorTreeTypes.h>
#include <AIWheeledVehicleController.h>
#include <WheeledVehicleMovementComponent4W.h>

EBTNodeResult::Type UBTTaskThrottle::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AAIWheeledVehicleController* AIController = Cast<AAIWheeledVehicleController>(OwnerComp.GetAIOwner());

	if (AIController)
	{
		AAIWheeledVehicleController* AIController = Cast<AAIWheeledVehicleController>(OwnerComp.GetAIOwner());
		//UBlackboardComponent* BlackboardComp = AIController->BlackboardComp;
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

		AIController->VehicleMovementComp->SetThrottleInput(BlackboardComp->GetValueAsFloat("ThrottleValue"));
		//AIController->VehicleMovementComp->SetThrottleInput(0.0f);

		//inform BT this node has successfully completed its task
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UBTTaskThrottle::OnGameplayTaskActivated(UGameplayTask & task)
{
	//This method must be overridden
	//Otherwise, the linker will fail.
}
