// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once
#include <GameFramework/GameModeBase.h>
#include <EngineUtils.h>
#include <AIWheeledVehicle.h>
#include <Powerup.h>
#include <Sound/SoundCue.h>
#include <Blueprint/UserWidget.h>
#include "VehicleTemplateGameMode.generated.h"

UCLASS(minimalapi)
class AVehicleTemplateGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<AActor*> SpawnPoints;

public:
	AVehicleTemplateGameMode();

	virtual void StartPlay() override;
	virtual void Tick(float DeltaTime) override;

	void CentreCursor();

	void PauseGame();
	void UnpauseGame();
	void EndGame();

	int GetCurrentWave();
	int GetEnemyCount();
	int GetNukeCount();

protected:
	UPROPERTY()
	TSubclassOf<class UUserWidget> CursorWidgetClass;

	UPROPERTY()
	TSubclassOf<class UUserWidget> PauseMenuWidgetClass;

	UPROPERTY()
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

	UPROPERTY()
	UUserWidget* CursorWidget;

	UPROPERTY()
	UUserWidget* PauseMenuWidget;

	UPROPERTY()
	UUserWidget* GameOverWidget;

private:
	USoundCue* GameOverSound;

	class AVehicleTemplatePawn* Player;
	class UNavigationSystem* NavSys;

	APlayerController* PlayerController;

	FVector MapCentre;
	float MapRadius;

	TSubclassOf<class AAIWheeledVehicle> StandardEnemy;
	TSubclassOf<class AAIWheeledVehicle> TankEnemy;
	TSubclassOf<class AAIWheeledVehicle> SpeedyEnemy;

	int TankEnemyWaveGap;
	int SpeedyEnemyWaveGap;

	float TankEnemyWaveRatio;
	float SpeedyEnemyWaveRatio;

	TSubclassOf<class APowerup> NukePowerup;
	TSubclassOf<class APowerup> SpeedBoostPowerup;
	TSubclassOf<class APowerup> SlomoPowerup;
	TSubclassOf<class APowerup> FreezePowerup;
	TSubclassOf<class APowerup> HealthBoostPowerup;

	int CurrentWave;
	TArray<AAIWheeledVehicle*> Enemies;

	float EnemySpawnZOffset;
	void SpawnEnemy(TSubclassOf<class AAIWheeledVehicle> EnemyType);

	/* When referring to powerups as 'other', these are types of powerup that aren't nukes,
	as nukes are treated differently to the rest of the powerup types */

	TArray<TSubclassOf<class APowerup>> OtherPowerupTypes;

	int MaxNukePowerups;
	int MaxOtherPowerups;

	TArray<APowerup*> NukePowerups;
	TArray<APowerup*> OtherPowerups;

	float PowerupZOffset;

	float NukePowerupSpawnDelay;
	float MinOtherPowerupSpawnDelay;
	float MaxOtherPowerupSpawnDelay;

	bool IsSpawningNukePowerup;
	bool IsSpawningOtherPowerup;

	APowerup* SpawnPowerup(TSubclassOf<class APowerup> PowerupType);
	void SpawnNukePowerup();
	void SpawnNukePowerup(float SpawnDelay);
	void SpawnRandomOtherPowerup();
	void SpawnRandomOtherPowerup(float SpawnDelay);
};
