// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <BehaviorTree/BTTaskNode.h>
#include "BTTaskThrottle.generated.h"

/**
*
*/
UCLASS()
class VEHICLETEMPLATE_API UBTTaskThrottle : public UBTTaskNode
{
	GENERATED_BODY()


		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& task) override;

};
