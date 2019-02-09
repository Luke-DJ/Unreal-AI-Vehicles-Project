#pragma once

#include <CoreMinimal.h>
#include <AIController.h>
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "AIWheeledVehicleController.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLETEMPLATE_API AAIWheeledVehicleController : public AAIController
{
	GENERATED_BODY()

public:
	//A cached vehicle movement component
	class UWheeledVehicleMovementComponent* VehicleMovementComp;

	//A cached BT component
	class UBehaviorTreeComponent* BehaviourTreeComp;

	//A cached Blackboard Component
	class UBlackboardComponent* BlackboardComp;

	AAIWheeledVehicleController();

	//override Possess to handle additional setup
	virtual void Possess(APawn* InPawn) override;
	
	//override Tick to create some autonomous behaviours
	virtual void Tick(float DeltaSeconds) override;
};
