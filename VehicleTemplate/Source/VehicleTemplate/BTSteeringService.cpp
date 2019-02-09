// Fill out your copyright notice in the Description page of Project Settings.

#include <BTSteeringService.h>
#include <EngineUtils.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <VehicleTemplatePawn.h>
#include <AIWheeledVehicle.h>
#include <WheeledVehicleMovementComponent.h>
#include <AIWheeledVehicleController.h>
#include <AI/Navigation/NavigationSystem.h>
#include <AI/Navigation/NavigationPath.h>
#include <DrawDebugHelpers.h>
#include <Powerup.h>


void UBTSteeringService::OnGameplayTaskActivated(UGameplayTask & task)
{
	//nothing here
	//This method must be overridden from the inherited interface
	//Otherwise, the linker will fail.
}

void UBTSteeringService::OnGameplayTaskDeactivated(UGameplayTask & task)
{
	//nothing here
	//This method must be overridden from the inherited interface
	//Otherwise, the linker will fail.
}

void UBTSteeringService::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	/*// A replacement for BeginPlay I had planned, since service nodes don't have this feature
	if (!IsSetup)
	{*/
		UWorld* World = GetWorld();
		TActorIterator<AVehicleTemplatePawn> PlayerPawnIter(World);
		PlayerPawn = *PlayerPawnIter;

		AIController = Cast<AAIWheeledVehicleController>(OwnerComp.GetAIOwner());
		BlackboardComp = AIController->BlackboardComp;
		//BlackboardComp = OwnerComp.GetBlackboardComponent();

		AIPawn = Cast<AAIWheeledVehicle>(AIController->GetPawn());
		if (AIPawn)
		{
			MinSteering = AIPawn->MinSteering;
			MaxSteering = AIPawn->MaxSteering;
			MinThrottle = AIPawn->MinThrottle;
			MaxThrottle = AIPawn->MaxThrottle;
		}
		else
		{
			// Using some defualt values if the actual values can't be retrieved
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Min/Max steering/throttle values couldn't be retrieved\nDefault values will be used"));
			MinSteering = 0.0f;
			MaxSteering = 1.5f;
			MinThrottle = 0.5f;
			MaxThrottle = 1.5f;
		}

		NavSys = UNavigationSystem::GetCurrent(PlayerPawn);

		//IsSetup = true;
	//}

	PlayerLocation = PlayerPawn->GetTransform().GetTranslation();
	EnemyLocation = AIController->GetPawn()->GetTransform().GetTranslation();

	NavPath = NavSys->FindPathToLocationSynchronously(World, EnemyLocation, PlayerLocation);
	// If the NavMesh can find a path to the player, set the first point in this path to the target location
	if (NavPath->PathPoints.Num() >= 2)
	{
		//[1], because [0] is the AI vehicle's location
		TargetLocation = NavPath->PathPoints[1];
	}
	// Otherwise, set the player's location to the target location
	else
	{
		TargetLocation = PlayerLocation;
	}

	float PowerupTargetDistance = 1000.0f;
	float PowerupMaxAngle = 45.0f;
	APowerup* TargetPowerup = nullptr;
	for (TActorIterator<APowerup> PowerupItr(World); PowerupItr; ++PowerupItr)
	{
		FVector CurrentPowerupLocation = (*PowerupItr)->GetTransform().GetTranslation();
		float CurrentPowerupDistance = (CurrentPowerupLocation - EnemyLocation).Size();

		// If the current powerup is within the given targeting distance
		if (CurrentPowerupDistance <= PowerupTargetDistance)
		{
			int CurrentPowerupPathPoints = NavSys->FindPathToLocationSynchronously(World, EnemyLocation, CurrentPowerupLocation)->PathPoints.Num();

			// If the path to the current powerup is uninterrupted
			if (CurrentPowerupPathPoints == 2)
			{
				float DegreeAngle = FMath::RadiansToDegrees(acos(FVector::DotProduct(EnemyForwardVector, CurrentPowerupLocation)));

				// If the current powerup is within the given targeting angle
				if (DegreeAngle <= PowerupMaxAngle)
				{
					// If there isn't already an existing viable powerup
					if (!TargetPowerup)
					{
						TargetPowerup = *PowerupItr;
					}
					// If the current powerup is closer than the existing viable powerup
					else if (CurrentPowerupDistance < (TargetPowerup->GetTransform().GetTranslation() - EnemyLocation).Size())
					{
						TargetPowerup = *PowerupItr;
					}
				}
			}
		}
	}
	if (TargetPowerup)
	{
		TargetLocation = TargetPowerup->GetTransform().GetTranslation();
	}

	// Debug draw for the navigation path
	for (int i = 0; i < NavPath->PathPoints.Num() - 1; i++)
	{
		DrawDebugLine(World, NavPath->PathPoints[i], NavPath->PathPoints[i + 1], FColor::Green, false, 0.5f, 0, 5);
	}

	EnemyForwardVector = AIController->GetPawn()->GetActorForwardVector();
	EnemyForwardVector.Normalize();
	EnemyRightVector = AIController->GetPawn()->GetActorRightVector();
	EnemyRightVector.Normalize();

	EnemyToTarget = TargetLocation - EnemyLocation;
	EnemyToTarget.Normalize();

	// Calculating the kilometres per second using information from the AI's movement component
	KPH = fabs(AIPawn->GetVehicleMovement()->GetForwardSpeed()) * 0.036f;

	// Setting up default steering and throttle values (will be adjusted later depending on certain factors)
	SteeringValue = FVector::DotProduct(EnemyRightVector, EnemyToTarget);
	ThrottleValue = FVector::DotProduct(EnemyForwardVector, EnemyToTarget);

	// If the AI vehicle is already reversing, allow use of the reversing manoeuvre
	if (AIPawn->GetVehicleMovement()->GetCurrentGear() < 0)
	{
		// Steering values will resemble the following: https://i.imgur.com/qvLMmJz.png
		// Throttle values will resemble the following: https://i.imgur.com/fUYD8mU.png

		/* If the target point is behind the AI vehicle, the steering value is set to its maximum value
		so that the AI vehicle can turn to face the target point quickly */
		if (ThrottleValue < 0.0f)
		{
			if (SteeringValue < 0.0f)
			{
				SteeringValue = 1.0f;
			}
			else
			{
				SteeringValue = -1.0f;
			}
		}
		// If the target point isn't behind the AI vehicle, then the throttle value is adjusted to match its desired value (https://i.imgur.com/fUYD8mU.png)
		else
		{
			ThrottleValue = 1.0f - ThrottleValue;
		}
	}
	// If the AI vehicle is 'stuck' (moving at a speed slower than 1KPH), then initiate a reversing manoeuvre
	else if (KPH <= 1.0f)
	{
		// Steering values will resemble the following: https://i.imgur.com/5pp7VoI.png

		if (SteeringValue < 0.0f)
		{
			SteeringValue = 1.0f;
		}
		else
		{
			SteeringValue = -1.0f;
		}

		// Forcing the throttle value to be negative (so that the AI vehicle will definitely reverse)
		ThrottleValue = -(fabs(ThrottleValue));
	}
	else
	{
		// If the target point is behind the AI vehicle
		if (ThrottleValue < 0.0f)
		{
			// Adjusting the steering value to match its desired value: https://i.imgur.com/jys7Amd.png
			if (SteeringValue < 0.0f)
			{
				SteeringValue = -(1.0f + (1.0f - fabs(SteeringValue)));
			}
			else
			{
				SteeringValue = 1.0f + (1.0f - SteeringValue);
			}

			float MinBrakeSpeed = 30.0f;
			/* If below a minimum braking speed, the throttle value is set to 0 (would previously have been a negative value),
			preventing the potential for the reversal manoeuvre to be triggered
			New values: https://i.imgur.com/Sicr0P0.png */
			if (KPH <= MinBrakeSpeed)
			{
				ThrottleValue = 0.0f;
			}
			// Otherwise, throttle values will resemble the following: https://i.imgur.com/USOpXmy.png
		}
		SteeringValue *= 0.5f;
		/* Alternative version of the steering value
		if (ThrottleValue < 0.0f)
		{
			if (SteeringValue < 0.0f)
			{
				SteeringValue = -1.0f;
			}
			else
			{
				SteeringValue = 1.0f;
			}

			if (KPH <= 50.0f)
			{
				ThrottleValue = 0.0f;
			}
		}*/
	}
	
	// Additional modification to ThrottleValue, to ensure the AI vehicle turns corners at a reliable speed
	float BrakeValue = 1.0f;
	BrakeValue *= -1.0f;
	float DistanceWithin = 1000.0f;
	float CumulativeDistanceMin = 500.0f;
	float MaxCornerSpeed = 15.0f;
	float ActualDistance = NavPath->GetPathLength();
	if (NavPath->PathPoints.Num() > 2 && ActualDistance < DistanceWithin)
	{
		int NextTargetPoint = 2;
		float CumulativeDistance = 0.0f;
		for (; NextTargetPoint < NavPath->PathPoints.Num() - 1; NextTargetPoint++)
		{
			CumulativeDistance += (NavPath->PathPoints[NextTargetPoint - 1] - NavPath->PathPoints[NextTargetPoint]).Size();
			if (CumulativeDistance > CumulativeDistanceMin)
			{
				break;
			}
		}

		float test = 180.0f - FMath::RadiansToDegrees(AngleFromThreePoints(EnemyLocation, NavPath->PathPoints[1], NavPath->PathPoints[NextTargetPoint]));
		if (test > 45.0f)
		{
			if (KPH > MaxCornerSpeed)
			{
				ThrottleValue = BrakeValue;
			}
			else
			{
				// Setting ThrottleValue to be within its Min & Max bounds
				ThrottleValue = ConvertToMinMax(MinThrottle, MinThrottle, MaxThrottle);
			}
		}
		else
		{
			// Setting ThrottleValue to be within its Min & Max bounds
			ThrottleValue = ConvertToMinMax(ThrottleValue, MinThrottle, MaxThrottle);
		}
	}
	else
	{
		// Setting ThrottleValue to be within its Min & Max bounds
		ThrottleValue = ConvertToMinMax(ThrottleValue, MinThrottle, MaxThrottle);
	}



	// Setting SteeringValue to be within its Min & Max bounds
	SteeringValue = ConvertToMinMax(SteeringValue, MinSteering, MaxSteering);

	/* Output for the calculated steering and throttle values
	GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, (TEXT("%f"), FString::SanitizeFloat(SteeringValue)));
	GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, (TEXT("%f"), FString::SanitizeFloat(ThrottleValue))); */

	/* Determines whether or not a location is on the NavMesh (currently unused, but could still be useful)
	FNavLocation NavLocation;
	bool OnNavMesh = NavSys->ProjectPointToNavigation(PlayerLocation, NavLocation);*/

	// Setting the steering and throttle values in the blackboard
	BlackboardComp->SetValueAsFloat("SteeringValue", SteeringValue);
	BlackboardComp->SetValueAsFloat("ThrottleValue", ThrottleValue);
}

float UBTSteeringService::ConvertToMinMax(float Value, float NewMin, float NewMax)
{
	if (Value < 0.0f)
	{
		return -((fabs(Value) * (NewMax - NewMin)) + NewMin);
	}
	else
	{
		return (Value * (NewMax - NewMin)) + NewMin;
	}
}

float UBTSteeringService::ConvertToMinMax(float Value, float OriginalMin, float OriginalMax, float NewMin, float NewMax)
{
	if (Value < 0.0f)
	{
		return -((((fabs(Value) - OriginalMin) * (NewMax - NewMin)) / (OriginalMax - OriginalMin)) + NewMin);
	}
	else
	{
		return (((Value - OriginalMin) * (NewMax - NewMin)) / (OriginalMax - OriginalMin)) + NewMin;
	}
}

// Returns the angle in radians calculated from three given points
float UBTSteeringService::AngleFromThreePoints(FVector PointOne, FVector PointTwo, FVector PointThree)
{
	FVector EnemyToFirstTarget = PointOne - PointTwo;
	EnemyToFirstTarget.Normalize();

	FVector SecondTargetToFirstTarget = PointThree - PointTwo;
	SecondTargetToFirstTarget.Normalize();

	return acos(FVector::DotProduct(EnemyToFirstTarget, SecondTargetToFirstTarget));
}
