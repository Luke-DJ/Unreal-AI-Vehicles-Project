// Fill out your copyright notice in the Description page of Project Settings.

#include <AIWheeledVehicle.h>
#include <VehicleTemplateWheelFront.h>
#include <VehicleTemplateWheelRear.h>
#include <WheeledVehicleMovementComponent4W.h>
#include <UObject/ConstructorHelpers.h>
#include <Components/SkeletalMeshComponent.h>
#include <Engine/Engine.h>
#include <Components/BoxComponent.h>
#include <AI/Navigation/NavigationSystem.h>
#include <VehicleTemplatePawn.h>


AAIWheeledVehicle::AAIWheeledVehicle()
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	// Simulation
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);

	Vehicle4W->WheelSetups[0].WheelClass = UVehicleTemplateWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UVehicleTemplateWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("Wheel_Front_Right");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = UVehicleTemplateWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("Wheel_Rear_Left");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = UVehicleTemplateWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("Wheel_Rear_Right");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	// Creating a subobject for the box collider
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
}

void AAIWheeledVehicle::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Setting the properties of the box collider component that allow for overlap events to occur
	BoxComponent->bGenerateOverlapEvents = true;
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAIWheeledVehicle::OnBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AAIWheeledVehicle::OnEndOverlap);
	// Attaching the box collider component to the root component
	BoxComponent->AttachTo(GetRootComponent(), TEXT("Box Collider"), EAttachLocation::KeepRelativeOffset, false);
	/* For debugging
	BoxComponent->SetHiddenInGame(false);
	BoxComponent->ShapeColor = FColor::Green; */
}

void AAIWheeledVehicle::Tick(float Delta)
{
	Super::Tick(Delta);
}

void AAIWheeledVehicle::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	AVehicleTemplatePawn* Player = Cast<AVehicleTemplatePawn>(OtherActor);

	/* Ensuring that the overlap occured with the player,
	decreasing the player's health according to the DamageToPlayer value,
	and 'killing' this enemy */
	if (Player && OtherComponent)
	{
		Player->DecreaseHealth(DamageToPlayer);

		Kill();
	}
}

void AAIWheeledVehicle::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	/* Unused, since there are currently no events that require something to take place once the overlap has ended
	
	AVehicleTemplatePawn* Player = Cast<AVehicleTemplatePawn>(OtherActor);

	if (Player && OtherComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, TEXT("End Overlap"));
	} */
}

void AAIWheeledVehicle::Kill()
{
	// Spawning a given particle system, and playing a given sound, at the enemy's final location
	UGameplayStatics::SpawnEmitterAtLocation(Cast<UObject>(GetWorld()), ExplosionParticleSystem, GetActorLocation(), GetActorRotation(), GetActorScale());
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetMesh()->GetComponentTransform().GetTranslation());
	// Destroying this enemy object
	Destroy();
}
