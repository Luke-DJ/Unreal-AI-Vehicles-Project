// Fill out your copyright notice in the Description page of Project Settings.

#include <MenuController.h>
#include <Blueprint/UserWidget.h>


void AMenuController::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidgetClass)
	{
		// Creating, storing, and adding the main menu widget to the viewport
		MainMenuWidget = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		MainMenuWidget->AddToViewport();
		// Setting the cursor to show on the screen
		bShowMouseCursor = true;
	}
}
