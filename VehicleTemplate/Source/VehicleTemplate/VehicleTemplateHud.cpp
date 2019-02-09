// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleTemplateHud.h"
#include "VehicleTemplatePawn.h"
#include "WheeledVehicle.h"
#include "RenderResource.h"
#include "Shader.h"
#include "Engine/Canvas.h"
#include "WheeledVehicleMovementComponent.h"
#include "Engine/Font.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include <VehicleTemplateGameMode.h>

#define LOCTEXT_NAMESPACE "VehicleHUD"

AVehicleTemplateHud::AVehicleTemplateHud()
{
	//static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
	static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Game/Fonts/Space_Ranger/spacerangeracad_Font"));
	HUDFont = Font.Object;
}

void AVehicleTemplateHud::DrawHUD()
{
	Super::DrawHUD();

	// Calculate ratio from 720p
	const float HUDXRatio = Canvas->SizeX / 1280.f;
	const float HUDYRatio = Canvas->SizeY / 720.f;

	bool bWantHUD = true;
#if HMD_MODULE_INCLUDED
	bWantHUD = !GEngine->IsStereoscopic3D();
#endif // HMD_MODULE_INCLUDED
	// We dont want the onscreen hud when using a HMD device	
	if (bWantHUD == true)
	{
		// Get our vehicle so we can check if we are in car. If we are we don't want onscreen HUD
		AVehicleTemplatePawn* Vehicle = Cast<AVehicleTemplatePawn>(GetOwningPawn());
		// Get the game mode so the current wave can be retrieved
		AVehicleTemplateGameMode* GameMode = Cast<AVehicleTemplateGameMode>(GetWorld()->GetAuthGameMode());
		if ((Vehicle != nullptr) && (Vehicle->bInCarCameraActive == false))
		{
			//FVector2D ScaleVec(HUDYRatio * 1.4f, HUDYRatio * 1.4f);
			FVector2D ScaleVec(HUDYRatio, HUDYRatio);

			// Speed
			FCanvasTextItem SpeedTextItem(FVector2D(HUDXRatio * 1075.0f, HUDYRatio * 625.0f), Vehicle->SpeedDisplayString, HUDFont, FLinearColor::White);
			SpeedTextItem.Scale = ScaleVec;
			Canvas->DrawItem(SpeedTextItem);

			// Gear
			FCanvasTextItem GearTextItem(FVector2D(HUDXRatio * 1075.0f, HUDYRatio * 670.0f), Vehicle->GearDisplayString, HUDFont, Vehicle->bInReverseGear == false ? Vehicle->GearDisplayColor : Vehicle->GearDisplayReverseColor);
			GearTextItem.Scale = ScaleVec;
			Canvas->DrawItem(GearTextItem);

			// Health
			FLinearColor HealthTextColour = FLinearColor(FColor::Orange);
			if (Vehicle->GetHealth() < (Vehicle->MaxHealth * (1.0f / 3.0f)))
			{
				HealthTextColour = FLinearColor::Red;
			}
			else if (Vehicle->GetHealth() > (Vehicle->MaxHealth * (2.0f / 3.0f)))
			{
				HealthTextColour = FLinearColor::Green;
			}
			FCanvasTextItem HealthTextItem(FVector2D(HUDXRatio * 15.0f, HUDYRatio * 535.0f), Vehicle->HealthDisplayString, HUDFont, HealthTextColour);
			HealthTextItem.Scale = ScaleVec;
			Canvas->DrawItem(HealthTextItem);

			// Wave
			FText WaveDisplayString = FText::AsCultureInvariant(FString("Current Wave: " + FString::FromInt(GameMode->GetCurrentWave())));
			FCanvasTextItem WaveTextItem(FVector2D(HUDXRatio * 15.0f, HUDYRatio * 580.0f), WaveDisplayString, HUDFont, FLinearColor::White);
			WaveTextItem.Scale = ScaleVec;
			Canvas->DrawItem(WaveTextItem);

			// Enemy Count
			FText EnemyCountDisplayString = FText::AsCultureInvariant(FString("Enemy Count: " + FString::FromInt(GameMode->GetEnemyCount())));
			FCanvasTextItem EnemyCountTextItem(FVector2D(HUDXRatio * 15.0f, HUDYRatio * 625.0f), EnemyCountDisplayString, HUDFont, FLinearColor::White);
			EnemyCountTextItem.Scale = ScaleVec;
			Canvas->DrawItem(EnemyCountTextItem);

			// Nuke
			FText NukeDisplayString;
			FLinearColor NukeTextColour;
			if (GameMode->GetNukeCount() == 0)
			{
				NukeDisplayString = FText::AsCultureInvariant(FString("Nuke Unavailable"));
				NukeTextColour = FLinearColor::Red;
			}
			else
			{
				NukeDisplayString = FText::AsCultureInvariant(FString("Nuke Available"));
				NukeTextColour = FLinearColor::Green;
			}
			FCanvasTextItem NukeTextItem(FVector2D(HUDXRatio * 15.0f, HUDYRatio * 670.0f), NukeDisplayString, HUDFont, NukeTextColour);
			NukeTextItem.Scale = ScaleVec;
			Canvas->DrawItem(NukeTextItem);
		}
	}
}


#undef LOCTEXT_NAMESPACE
