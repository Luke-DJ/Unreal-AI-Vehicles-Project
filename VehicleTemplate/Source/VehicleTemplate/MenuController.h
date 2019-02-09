// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/PlayerController.h>
#include "MenuController.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLETEMPLATE_API AMenuController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	// Reference to the UMG asset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;

	// Variable to hold the created widget
	UUserWidget* MainMenuWidget;
};
