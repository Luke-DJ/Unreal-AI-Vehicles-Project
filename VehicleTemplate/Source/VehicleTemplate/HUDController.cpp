// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDController.h"
#include <Blueprint/UserWidget.h>
#include <ConstructorHelpers.h>
#include <Kismet/GameplayStatics.h>


AHUDController::AHUDController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/UI/GameOver"));
	if (WidgetClass.Class != NULL)
	{
		GameOverWidgetClass = WidgetClass.Class;
	}
}

void AHUDController::BeginPlay()
{
	Super::BeginPlay();

	if (GameOverWidgetClass)
	{
		// Creating and storing the main menu widget
		GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
	}
}

void AHUDController::DisplayGameOver()
{
	// Adding the main menu widget to the viewport
	if (GameOverWidget)
	{
		//if (!GameOverWidget->IsInViewport())
		//{
			GameOverWidget->AddToViewport();
		//}

		// Setting the cursor to show on the screen

		//bShowMouseCursor = true;

		/*if (!UGameplayStatics::IsGamePaused(GetWorld()))
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}*/
	}
}

void AHUDController::HideGameOver()
{
	// Removing the main menu widget from the viewport
	if (GameOverWidget)
	{
		//if (GameOverWidget->IsInViewport())
		//{
			GameOverWidget->RemoveFromViewport();
		//}

		// Hiding the cursor from the screen
		//bShowMouseCursor = false;

		/*if (UGameplayStatics::IsGamePaused(GetWorld()))
		{
			UGameplayStatics::SetGamePaused(GetWorld(), false);
		}*/
	}
}
