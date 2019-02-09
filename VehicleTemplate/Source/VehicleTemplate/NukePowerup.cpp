// Fill out your copyright notice in the Description page of Project Settings.

#include <NukePowerup.h>
#include <EngineUtils.h>
#include <VehicleTemplatePawn.h>
#include <AIWheeledVehicle.h>


void ANukePowerup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);

	// Triggering the destruction of all enemies on overlap
	if (Cast<AVehicleTemplatePawn>(OtherActor) && OtherComponent)
	{
		DestroyAllEnemies();
		Destroy();
	}
}

void ANukePowerup::DestroyAllEnemies()
{
	/* The iterator I have used: https://wiki.unrealengine.com/Iterators:_Object_%26_Actor_Iterators,_Optional_Class_Scope_For_Faster_Search#Actor_Iterator
	Iterating over all enemies, 'killing' each one */
	for (TActorIterator<AAIWheeledVehicle> EnemyItr(GetWorld()); EnemyItr; ++EnemyItr)
	{
		(*EnemyItr)->Kill();
	}
}
