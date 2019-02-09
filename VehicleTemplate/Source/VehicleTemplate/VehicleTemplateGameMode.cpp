// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include <VehicleTemplateGameMode.h>
#include <VehicleTemplatePawn.h>
#include <VehicleTemplateHud.h>
#include <AIWheeledVehicle.h>
#include <SpawnPoint.h>
#include <AI/Navigation/NavigationSystem.h>
#include <ConstructorHelpers.h>
#include <TimerManager.h>
#include <DrawDebugHelpers.h>


AVehicleTemplateGameMode::AVehicleTemplateGameMode()
{
	DefaultPawnClass = AVehicleTemplatePawn::StaticClass();
	HUDClass = AVehicleTemplateHud::StaticClass();

	MapCentre = FVector(50.0f, 50.0f, 115.0f);
	MapRadius = 15000.0f;

	static ConstructorHelpers::FClassFinder<UUserWidget> CWidgetClass(TEXT("/Game/UI/CustomMouseCursor"));
	CursorWidgetClass = CWidgetClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> PMWidgetClass(TEXT("/Game/UI/PauseMenu"));
	PauseMenuWidgetClass = PMWidgetClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> GOWidgetClass(TEXT("/Game/UI/GameOver"));
	GameOverWidgetClass = GOWidgetClass.Class;

	static ConstructorHelpers::FObjectFinder<USoundCue> GameOverSoundClass(TEXT("/Game/Audio/Game_Over_Cue"));
	GameOverSound = GameOverSoundClass.Object;

	static ConstructorHelpers::FClassFinder<AAIWheeledVehicle> StandardEnemyClass(TEXT("/Game/StandardEnemy"));
	StandardEnemy = StandardEnemyClass.Class;

	static ConstructorHelpers::FClassFinder<AAIWheeledVehicle> TankEnemyClass(TEXT("/Game/TankEnemy"));
	TankEnemy = TankEnemyClass.Class;

	static ConstructorHelpers::FClassFinder<AAIWheeledVehicle> SpeedyEnemyClass(TEXT("/Game/SpeedyEnemy"));
	SpeedyEnemy = SpeedyEnemyClass.Class;

	static ConstructorHelpers::FClassFinder<APowerup> NukePowerupClass(TEXT("/Game/Powerups/BP_NukePowerup"));
	NukePowerup = NukePowerupClass.Class;

	static ConstructorHelpers::FClassFinder<APowerup> SpeedBoostPowerupClass(TEXT("/Game/Powerups/BP_SpeedBoostPowerup"));
	SpeedBoostPowerup = SpeedBoostPowerupClass.Class;

	static ConstructorHelpers::FClassFinder<APowerup> SlomoPowerupClass(TEXT("/Game/Powerups/BP_SlomoPowerup"));
	SlomoPowerup = SlomoPowerupClass.Class;

	static ConstructorHelpers::FClassFinder<APowerup> FreezePowerupClass(TEXT("/Game/Powerups/BP_FreezePowerup"));
	FreezePowerup = FreezePowerupClass.Class;

	static ConstructorHelpers::FClassFinder<APowerup> HealthBoostPowerupClass(TEXT("/Game/Powerups/BP_HealthBoostPowerup"));
	HealthBoostPowerup = HealthBoostPowerupClass.Class;

	CurrentWave = 0;
	Enemies = {};

	EnemySpawnZOffset = 50.0f;

	TankEnemyWaveGap = 5;
	SpeedyEnemyWaveGap = 3;

	TankEnemyWaveRatio = 1.0f / (float)TankEnemyWaveGap;
	SpeedyEnemyWaveRatio = 1.0f / (float)SpeedyEnemyWaveGap;

	OtherPowerupTypes = {SpeedBoostPowerup, SlomoPowerup, FreezePowerup, HealthBoostPowerup};

	MaxNukePowerups = 1;
	MaxOtherPowerups = 4;

	NukePowerups = {};
	OtherPowerups = {};

	PowerupZOffset = 100.0f;

	NukePowerupSpawnDelay = 10.0f;
	MinOtherPowerupSpawnDelay = 15.0f;
	MaxOtherPowerupSpawnDelay = 20.0f;

	IsSpawningNukePowerup = false;
	IsSpawningOtherPowerup = false;

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	SetTickableWhenPaused(false);
}

void AVehicleTemplateGameMode::StartPlay()
{
	Super::StartPlay();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// Creating and storing UI widgets
	if (CursorWidgetClass)
	{
		CursorWidget = CreateWidget<UUserWidget>(GetWorld(), CursorWidgetClass);
		PlayerController->SetMouseCursorWidget(EMouseCursor::Default, CursorWidget);
	}
	if (PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
		PauseMenuWidget->bIsFocusable = true;
	}
	if (GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
		GameOverWidget->bIsFocusable = true;
	}

	TActorIterator<AVehicleTemplatePawn> PlayerPawnIter(GetWorld());
	Player = *PlayerPawnIter;

	NavSys = UNavigationSystem::GetCurrent(Player);
}

void AVehicleTemplateGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ending the game if the player's health has hit 0
	if (Player->GetHealth() <= 0)
	{
		EndGame();
		return;
	}

	// Cleaning the Enemies array of destroyed enemies
	TArray<AAIWheeledVehicle*> DestroyedEnemies = {};
	for (int i = 0; i < Enemies.Num(); i++)
	{
		if (Enemies[i]->IsPendingKill())
		{
			DestroyedEnemies.Add(Enemies[i]);
		}
	}
	for (int i = 0; i < DestroyedEnemies.Num(); i++)
	{
		Enemies.Remove(DestroyedEnemies[i]);
	}

	/* Debug draw for random points on the NavMesh
	FNavLocation RandomNavPoint;
	NavSys->GetRandomPointInNavigableRadius(MapCentre, MapRadius, RandomNavPoint, NavSys->MainNavData);
	DrawDebugPoint(GetWorld(), RandomNavPoint.Location, 10.0f, FColor::Green, false, 2.0f); */

	// Spawning a new wave of enemies if all current enemies have been killed
	if (Enemies.Num() == 0)
	{
		CurrentWave++;

		for (int i = 1; i <= CurrentWave; i++)
		{
			if (CurrentWave % TankEnemyWaveGap == 0 && floorf(TankEnemyWaveRatio * i) == TankEnemyWaveRatio * i)
			{
				SpawnEnemy(TankEnemy);
			}
			else if (CurrentWave % SpeedyEnemyWaveGap == 0 && floorf(SpeedyEnemyWaveRatio * i) == SpeedyEnemyWaveRatio * i)
			{
				SpawnEnemy(SpeedyEnemy);
			}
			else
			{
				SpawnEnemy(StandardEnemy);
			}
		}
	}

	// Cleaning the Powerup arrays of destroyed powerups
	TArray<APowerup*> DestroyedPowerups = {};
	for (int i = 0; i < NukePowerups.Num(); i++)
	{
		if (NukePowerups[i]->IsPendingKill())
		{
			DestroyedPowerups.Add(NukePowerups[i]);
		}
	}
	for (int i = 0; i < DestroyedPowerups.Num(); i++)
	{
		NukePowerups.Remove(DestroyedPowerups[i]);
	}
	DestroyedPowerups.Empty();
	for (int i = 0; i < OtherPowerups.Num(); i++)
	{
		if (OtherPowerups[i]->IsPendingKill() || !OtherPowerups[i]->MeshComponent->IsVisible())
		{
			DestroyedPowerups.Add(OtherPowerups[i]);
		}
	}
	for (int i = 0; i < DestroyedPowerups.Num(); i++)
	{
		OtherPowerups.Remove(DestroyedPowerups[i]);
	}

	// Begin spawning a new nuke powerup if one isn't already being spawned and the max number of nuke powerups hasn't been reached
	if (!IsSpawningNukePowerup && NukePowerups.Num() < MaxNukePowerups)
	{
		SpawnNukePowerup(NukePowerupSpawnDelay);
	}

	// Begin spawning a random non-nuke powerup if one isn't already being spawned and the max number of non-nuke powerups hasn't been reached
	if (!IsSpawningOtherPowerup && OtherPowerups.Num() < MaxOtherPowerups)
	{
		SpawnRandomOtherPowerup(FMath::RandRange(MinOtherPowerupSpawnDelay, MaxOtherPowerupSpawnDelay));
	}
}

// Refactored from: https://answers.unrealengine.com/questions/339494/how-to-set-mouse-position-in-center-in-c.html
void AVehicleTemplateGameMode::CentreCursor()
{
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (LocalPlayer)
	{
		UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient;
		if (ViewportClient)
		{
			FViewport* Viewport = ViewportClient->Viewport;
			if (Viewport)
			{
				FVector2D ViewportSize;
				ViewportClient->GetViewportSize(ViewportSize);
				int32 X = static_cast<int32>(ViewportSize.X * 0.5f);
				int32 Y = static_cast<int32>(ViewportSize.Y * 0.5f);
				Viewport->SetMouse(X, Y);
			}
		}
	}
}

void AVehicleTemplateGameMode::PauseGame()
{
	// Setting up the cursor
	PlayerController->bShowMouseCursor = true;
	CentreCursor();
	PlayerController->SetInputMode(FInputModeUIOnly());

	// Adding the pause menu widget to the viewport
	if (!PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->AddToViewport();
	}
	PauseMenuWidget->SetKeyboardFocus();

	// Setting the game to 'paused'
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AVehicleTemplateGameMode::UnpauseGame()
{
	// Setting up the cursor
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetInputMode(FInputModeGameOnly());

	// Removing the pause menu widget from the viewport
	if (PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->RemoveFromViewport();
	}

	// Setting the game to 'unpaused'
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void AVehicleTemplateGameMode::EndGame()
{
	// Setting up the cursor
	PlayerController->bShowMouseCursor = true;
	CentreCursor();
	PlayerController->SetInputMode(FInputModeUIOnly());

	// Adding the game over menu widget to the viewport
	if (!GameOverWidget->IsInViewport())
	{
		GameOverWidget->AddToViewport();
	}

	// Setting the game to 'paused'
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// Playing the game over sound
	UGameplayStatics::PlaySound2D(GetWorld(), GameOverSound);
}

int AVehicleTemplateGameMode::GetCurrentWave()
{
	return CurrentWave;
}

int AVehicleTemplateGameMode::GetEnemyCount()
{
	return Enemies.Num();
}

int AVehicleTemplateGameMode::GetNukeCount()
{
	return NukePowerups.Num();
}

void AVehicleTemplateGameMode::SpawnEnemy(TSubclassOf<class AAIWheeledVehicle> EnemyType)
{
	/* While loop used here, as there are certain circumstances where SpawnActor will fail when
	given a random navmesh point (e.g. enemy spawns inside the player and is immediately killed)
	Not ideal, but no feasible alternative */

	AAIWheeledVehicle* EnemyVehicleInstance = nullptr;
	FNavLocation RandomNavPoint;
	FActorSpawnParameters SpawnParams;
	while (!EnemyVehicleInstance)
	{
		NavSys->GetRandomPointInNavigableRadius(MapCentre, MapRadius, RandomNavPoint, NavSys->MainNavData);

		// Values that allow the enemy to be rotated to face the player when they're spawned
		FVector EnemyForwardVector = RandomNavPoint.Location.ForwardVector;
		EnemyForwardVector.Normalize();
		FVector EnemyRightVector = RandomNavPoint.Location.RightVector;
		EnemyRightVector.Normalize();
		FVector EnemyToPlayer = Player->GetTransform().GetTranslation() - RandomNavPoint.Location;
		EnemyToPlayer.Normalize();
		float AngleToPlayer = acos(FVector::DotProduct(EnemyForwardVector, EnemyToPlayer));
		if (FVector::DotProduct(EnemyRightVector, EnemyToPlayer) < 0.0f)
		{
			AngleToPlayer = (PI - AngleToPlayer) + PI;
		}
		AngleToPlayer = FMath::RadiansToDegrees(AngleToPlayer);

		FRotator Rotation = FRotator(0.0f, AngleToPlayer, 0.0f);

		RandomNavPoint.Location.Z += EnemySpawnZOffset;
		EnemyVehicleInstance = GetWorld()->SpawnActor<AAIWheeledVehicle>(EnemyType, RandomNavPoint.Location, Rotation, SpawnParams);
	}

	EnemyVehicleInstance->SpawnDefaultController();

	// Adding the enemy to the array of enemies
	Enemies.Add(EnemyVehicleInstance);
}

APowerup* AVehicleTemplateGameMode::SpawnPowerup(TSubclassOf<class APowerup> PowerupType)
{
	APowerup* PowerupInstance = nullptr;
	FNavLocation RandomNavPoint;
	FActorSpawnParameters SpawnParameters;
	while (!PowerupInstance)
	{
		NavSys->GetRandomPointInNavigableRadius(MapCentre, MapRadius, RandomNavPoint, NavSys->MainNavData);
		PowerupInstance = GetWorld()->SpawnActor<APowerup>(PowerupType, FVector(RandomNavPoint.Location.X, RandomNavPoint.Location.Y, RandomNavPoint.Location.Z + PowerupZOffset), FRotator(0.0f, 0.0f, 0.0f), SpawnParameters);
	}
	return PowerupInstance;
}

void AVehicleTemplateGameMode::SpawnNukePowerup()
{
	NukePowerups.Add(SpawnPowerup(NukePowerup));
	IsSpawningNukePowerup = false;
}

void AVehicleTemplateGameMode::SpawnNukePowerup(float SpawnDelay)
{
	IsSpawningNukePowerup = true;
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AVehicleTemplateGameMode::SpawnNukePowerup, SpawnDelay, false);
}

void AVehicleTemplateGameMode::SpawnRandomOtherPowerup()
{
	int RandomPowerupIndex = FMath::RandRange(0, OtherPowerupTypes.Num() - 1);
	OtherPowerups.Add(SpawnPowerup(OtherPowerupTypes[RandomPowerupIndex]));
	IsSpawningOtherPowerup = false;
}

void AVehicleTemplateGameMode::SpawnRandomOtherPowerup(float SpawnDelay)
{
	IsSpawningOtherPowerup = true;
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AVehicleTemplateGameMode::SpawnRandomOtherPowerup, SpawnDelay, false);
}
