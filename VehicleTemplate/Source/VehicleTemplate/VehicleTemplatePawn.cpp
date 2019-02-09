// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleTemplatePawn.h"
#include "VehicleTemplateWheelFront.h"
#include "VehicleTemplateWheelRear.h"
#include "VehicleTemplateHud.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/TextRenderComponent.h"
#include "Materials/Material.h"
#include "GameFramework/Controller.h"
#include <Kismet/GameplayStatics.h>

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#endif // HMD_MODULE_INCLUDED

const FName AVehicleTemplatePawn::LookUpBinding("LookUp");
const FName AVehicleTemplatePawn::LookRightBinding("LookRight");

#define LOCTEXT_NAMESPACE "VehiclePawn"

AVehicleTemplatePawn::AVehicleTemplatePawn()
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

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 200.f);
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 7.f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	// Create In-Car camera component 
	InternalCameraOrigin = FVector(0.0f, -40.0f, 120.0f);

	InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
	InternalCameraBase->SetRelativeLocation(InternalCameraOrigin);
	InternalCameraBase->SetupAttachment(GetMesh());

	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	InternalCamera->bUsePawnControlRotation = false;
	InternalCamera->FieldOfView = 90.f;
	InternalCamera->SetupAttachment(InternalCameraBase);

	//Setup TextRenderMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> TextMaterial(TEXT("Material'/Engine/EngineMaterials/AntiAliasedTextMaterialTranslucent.AntiAliasedTextMaterialTranslucent'"));
	
	UMaterialInterface* Material = TextMaterial.Object;

	// Create text render component for in car speed display
	InCarSpeed = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
	InCarSpeed->SetTextMaterial(Material);
	InCarSpeed->SetRelativeLocation(FVector(70.0f, -75.0f, 99.0f));
	InCarSpeed->SetRelativeRotation(FRotator(18.0f, 180.0f, 0.0f));
	InCarSpeed->SetupAttachment(GetMesh());
	InCarSpeed->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));

	// Create text render component for in car gear display
	InCarGear = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	InCarGear->SetTextMaterial(Material);
	InCarGear->SetRelativeLocation(FVector(66.0f, -9.0f, 95.0f));	
	InCarGear->SetRelativeRotation(FRotator(25.0f, 180.0f,0.0f));
	InCarGear->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));
	InCarGear->SetupAttachment(GetMesh());
	
	// Colors for the incar gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	// Colors for the in-car gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	bInReverseGear = false;
}

void AVehicleTemplatePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AVehicleTemplatePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVehicleTemplatePawn::MoveRight);
	PlayerInputComponent->BindAxis("LookUp");
	PlayerInputComponent->BindAxis("LookRight");

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AVehicleTemplatePawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AVehicleTemplatePawn::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AVehicleTemplatePawn::OnToggleCamera);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AVehicleTemplatePawn::OnResetVR);

	PlayerInputComponent->BindAction("ResetPlayerRotation", IE_Pressed, this, &AVehicleTemplatePawn::OnResetPlayerRotation);

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AVehicleTemplatePawn::OnPause);

	// New camera inputs
	PlayerInputComponent->BindAxis("LookUp", this, &AVehicleTemplatePawn::PitchCamera);
	PlayerInputComponent->BindAxis("LookRight", this, &AVehicleTemplatePawn::YawCamera);
}

void AVehicleTemplatePawn::MoveForward(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void AVehicleTemplatePawn::MoveRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void AVehicleTemplatePawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AVehicleTemplatePawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AVehicleTemplatePawn::OnToggleCamera()
{
	EnableIncarView(!bInCarCameraActive);
}

void AVehicleTemplatePawn::OnResetPlayerRotation()
{
	// Allows the player to reset the rotation of their vehicle if it's upside-down
	// Getting the vehicle's current rotation
	FRotator VehicleRotation = GetActorRotation();
	// Creating booleans that indicate whether or not the vehicle is upside-down
	bool PitchUpsideDown = VehicleRotation.Pitch <= -90.0f || VehicleRotation.Pitch >= 90.0f;
	bool RollUpsideDown = VehicleRotation.Roll <= -90.0f || VehicleRotation.Roll >= 90.0f;
	// If either boolean is true, the pitch/roll of the vehicle is reset to 0 accordingly
	if (PitchUpsideDown || RollUpsideDown)
	{
		if (PitchUpsideDown && RollUpsideDown)
		{
			SetActorRotation(FRotator(0.0f, VehicleRotation.Yaw, 0.0f), ETeleportType::TeleportPhysics);
		}
		else if (PitchUpsideDown)
		{
			SetActorRotation(FRotator(0.0f, VehicleRotation.Yaw, VehicleRotation.Roll), ETeleportType::TeleportPhysics);
		}
		else if (RollUpsideDown)
		{
			SetActorRotation(FRotator(VehicleRotation.Pitch, VehicleRotation.Yaw, 0.0f), ETeleportType::TeleportPhysics);
		}
		// Resetting the velocity of the vehicle to avoid becoming stuck in an endless rotation reset loop
		GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector(0.0f, 0.0f, 0.0f), false);
		GetMesh()->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, 0.0f), false);
	}
}

void AVehicleTemplatePawn::OnPause()
{
	// Allows the player to pause/un-pause the game
	if (!UGameplayStatics::IsGamePaused(GetWorld()))
	{
		GameMode->PauseGame();
	}
	else
	{
		GameMode->UnpauseGame();
	}
}

void AVehicleTemplatePawn::EnableIncarView(const bool bState, const bool bForce)
{
	if ((bState != bInCarCameraActive) || ( bForce == true ))
	{
		bInCarCameraActive = bState;
		
		if (bState == true)
		{
			OnResetVR();
			Camera->Deactivate();
			InternalCamera->Activate();
		}
		else
		{
			InternalCamera->Deactivate();
			Camera->Activate();
		}
		
		InCarSpeed->SetVisibility(bInCarCameraActive);
		InCarGear->SetVisibility(bInCarCameraActive);
	}
}

//Methods for providing CameraInput with pitch/yaw camera information

void AVehicleTemplatePawn::PitchCamera(float AxisValue)
{
	CameraInput.Y = AxisValue;
}

void AVehicleTemplatePawn::YawCamera(float AxisValue)
{
	CameraInput.X = AxisValue;
}

void AVehicleTemplatePawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
	
	// Update the strings used in the hud (incar and onscreen)
	UpdateHUDStrings();

	// Set the string in the incar hud
	SetupInCarHUD();

	bool bHMDActive = false;

	// Set the new spring arm position using CameraInput
	FRotator NewRotation = SpringArm->GetComponentRotation();
	NewRotation.Roll = 0.0f;  // Assignment of Roll value is irrelevant, as camera inherits Roll from parent
	NewRotation.Pitch += CameraInput.Y;
	NewRotation.Yaw += CameraInput.X;
	// Keeping the Yaw value within 360 degrees
	if (NewRotation.Yaw > 360.0f)
	{
		NewRotation.Yaw -= 360.0f;
	}
	else if (NewRotation.Yaw < 0.0f)
	{
		NewRotation.Yaw += 360.0f;
	}
	SpringArm->SetWorldRotation(NewRotation);

#if HMD_MODULE_INCLUDED
	if ((GEngine->XRSystem.IsValid() == true) && ((GEngine->XRSystem->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true)))
	{
		bHMDActive = true;
	}
#endif // HMD_MODULE_INCLUDED
	if (bHMDActive == false)
	{
		if ( (InputComponent) && (bInCarCameraActive == true ))
		{
			FRotator HeadRotation = InternalCamera->RelativeRotation;
			HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
			HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
			InternalCamera->RelativeRotation = HeadRotation;
		}
	}
}

void AVehicleTemplatePawn::BeginPlay()
{
	Super::BeginPlay();

	bool bEnableInCar = false;
#if HMD_MODULE_INCLUDED
	bEnableInCar = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
#endif // HMD_MODULE_INCLUDED
	EnableIncarView(bEnableInCar,true);

	// Getting a reference to the game mode class
	GameMode = Cast<AVehicleTemplateGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	// Setting the initial health value to the maximum health value
	Health = MaxHealth;
}

void AVehicleTemplatePawn::OnResetVR()
{
#if HMD_MODULE_INCLUDED
	if (GEngine->XRSystem.IsValid())
	{
		GEngine->XRSystem->ResetOrientationAndPosition();
		InternalCamera->SetRelativeLocation(InternalCameraOrigin);
		GetController()->SetControlRotation(FRotator());
	}
#endif // HMD_MODULE_INCLUDED
}

void AVehicleTemplatePawn::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));
	
	if (bInReverseGear == true)
	{
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	}
	else
	{
		int32 Gear = GetVehicleMovement()->GetCurrentGear();
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}	

	HealthDisplayString = FText::Format(LOCTEXT("HealthFormat", "Health: {0}"), FText::AsNumber(Health));
}

void AVehicleTemplatePawn::SetupInCarHUD()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if ((PlayerController != nullptr) && (InCarSpeed != nullptr) && (InCarGear != nullptr) )
	{
		// Setup the text render component strings
		InCarSpeed->SetText(SpeedDisplayString);
		InCarGear->SetText(GearDisplayString);
		
		if (bInReverseGear == false)
		{
			InCarGear->SetTextRenderColor(GearDisplayColor);
		}
		else
		{
			InCarGear->SetTextRenderColor(GearDisplayReverseColor);
		}
	}
}

// Getter for the player's health value
int AVehicleTemplatePawn::GetHealth()
{
	return Health;
}

// Decreases the player's health value by a given amount
void AVehicleTemplatePawn::DecreaseHealth(int HealthDecrease)
{
	// Ensuring the health value doesn't fall below 0
	if (Health - HealthDecrease <= 0)
	{
		Health = 0;
	}
	else
	{
		Health -= HealthDecrease;
	}
}

// Increases the player's health value by a given amount
void AVehicleTemplatePawn::IncreaseHealth(int HealthIncrease)
{
	// Ensuring the health value doesn't go above the maximum amount of health
	if (Health + HealthIncrease >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += HealthIncrease;
	}
}

#undef LOCTEXT_NAMESPACE
