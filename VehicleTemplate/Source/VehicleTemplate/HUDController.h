// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/PlayerController.h>
#include "HUDController.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLETEMPLATE_API AHUDController : public APlayerController
{
	GENERATED_BODY()

public:
	AHUDController();

	virtual void BeginPlay() override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

	UUserWidget* GameOverWidget;

	void DisplayGameOver();
	void HideGameOver();
};
