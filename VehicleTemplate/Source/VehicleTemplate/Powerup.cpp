// Fill out your copyright notice in the Description page of Project Settings.

#include <Powerup.h>
#include <VehicleTemplatePawn.h>
#include <AIWheeledVehicle.h>


APowerup::APowerup()
{
	// Creating the powerup's capsule collider
	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	// Ensuring the capsule collider is set to the location of the powerup in the level
	CapsuleCollider->SetWorldTransform(GetTransform());
	// Allowing for overlap events
	CapsuleCollider->bGenerateOverlapEvents = true;
	CapsuleCollider->OnComponentBeginOverlap.AddDynamic(this, &APowerup::OnBeginOverlap);
	/* For debugging
	CapsuleCollider->SetHiddenInGame(false);
	CapsuleCollider->ShapeColor = FColor::Green; */

	// Creating the powerup's mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	// Ensuring the mesh is set to the location of the powerup in the level
	MeshComponent->SetWorldTransform(CapsuleCollider->GetComponentTransform());
	// Ignoring collisions on the mesh
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// Attaching the mesh to the capsule collider
	MeshComponent->AttachToComponent(CapsuleCollider, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));

	// Allowing for the Tick method to be used
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void APowerup::BeginPlay()
{
	Super::BeginPlay();

	// Playing a spawn sound for the powerup
	UGameplayStatics::PlaySoundAtLocation(this, SpawnInSound, MeshComponent->GetComponentTransform().GetTranslation());

	// Initial values for the 'bobbing' effect
	OriginalZ = GetActorLocation().Z;
	LastZ = OriginalZ;
}

void APowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Adding the spin for this frame, using SpinValue
	AddActorLocalRotation(SpinValue * DeltaTime);

	float DiffZ = GetActorLocation().Z - LastZ;

	OriginalZ += DiffZ;

	// Applying the 'bobbing' effect for this frame, using BobbingSpeed and BobbingStrength
	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, OriginalZ + (sin(GetGameTimeSinceCreation() * BobbingSpeed) * BobbingStrength)));

	LastZ = GetActorLocation().Z;
}

void APowerup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if ((Cast<AVehicleTemplatePawn>(OtherActor) || Cast<AAIWheeledVehicle>(OtherActor)) && OtherComponent)
	{
		// Spawning a particle system at the powerup's last location, playing a sound at the powerup's last location, and destroying the powerup on overlap
		UGameplayStatics::SpawnEmitterAtLocation(Cast<UObject>(GetWorld()), PickupParticleSystem, GetActorLocation(), GetActorRotation(), GetActorScale());
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, MeshComponent->GetComponentTransform().GetTranslation());

		if (Cast<AAIWheeledVehicle>(OtherActor))
		{
			Destroy();
		}
	}
}
