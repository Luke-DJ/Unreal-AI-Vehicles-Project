// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <BehaviorTree/BTTaskNode.h>
#include "BTTaskSteerVehicle.generated.h"

/**
*
*/
UCLASS()
class VEHICLETEMPLATE_API UBTTaskSteerVehicle : public UBTTaskNode
{
	GENERATED_BODY()


public:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& task) override;
};
