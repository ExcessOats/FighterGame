// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterPlayerController.h"
#include "Fighter/GameMode/FGGameMode.h"
#include "Fighter/Character/FighterCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "Fighter/Camera/FightCamera.h"
#include "Fighter/HUD/CharacterOverlay.h"
#include "Fighter/HUD/FighterHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void AFighterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FighterHUD = Cast<AFighterHUD>(GetHUD());
}

void AFighterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PollInit();
}

void AFighterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AFighterCharacter* FighterCharacter = Cast<AFighterCharacter>(InPawn);
	if (FighterCharacter)
	{
		if (FighterCharacter->GetFighterContext())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
			{
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(FighterCharacter->GetFighterContext(), 0);
			}
		}
		if (FighterCharacter->OtherPlayer)
		{
			if (FighterCharacter->bStartsLeft)
			{
				SetHUDHealthP1(FighterCharacter->GetHealth(), FighterCharacter->GetMaxHealth());
				SetHUDHealthP2(FighterCharacter->OtherPlayer->GetHealth(), FighterCharacter->OtherPlayer->GetMaxHealth());
			}
			else
			{
				SetHUDHealthP1(FighterCharacter->OtherPlayer->GetHealth(), FighterCharacter->OtherPlayer->GetMaxHealth());
				SetHUDHealthP2(FighterCharacter->GetHealth(), FighterCharacter->GetMaxHealth());
			}
		}
		
		if (FighterCharacter->GetFightCamera())
		{
			SetViewTargetWithBlend(FighterCharacter->GetFightCamera());
		}
	}
}

void AFighterPlayerController::SetHUDHealthP1(float Health, float MaxHealth)
{
	FighterHUD = FighterHUD == nullptr ? Cast<AFighterHUD>(GetHUD()) : FighterHUD;
	bool bHUDValid = FighterHUD && 
		FighterHUD->CharacterOverlay && 
		FighterHUD->CharacterOverlay->HealthBarP1 && 
		FighterHUD->CharacterOverlay->HealthTextP1;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		FighterHUD->CharacterOverlay->HealthBarP1->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Health));
		FighterHUD->CharacterOverlay->HealthTextP1->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeHealthP1 = true;
		HUDHealthP1 = Health;
		HUDMaxHealthP1 = MaxHealth;
	}
}

void AFighterPlayerController::SetHUDHealthP2(float Health, float MaxHealth)
{
	FighterHUD = FighterHUD == nullptr ? Cast<AFighterHUD>(GetHUD()) : FighterHUD;
	bool bHUDValid = FighterHUD &&
		FighterHUD->CharacterOverlay &&
		FighterHUD->CharacterOverlay->HealthBarP2 &&
		FighterHUD->CharacterOverlay->HealthTextP2;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		FighterHUD->CharacterOverlay->HealthBarP2->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Health));
		FighterHUD->CharacterOverlay->HealthTextP2->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeHealthP2 = true;
		HUDHealthP2 = Health;
		HUDMaxHealthP2 = MaxHealth;
	}
}

void AFighterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (FighterHUD && FighterHUD->CharacterOverlay)
		{
			CharacterOverlay = FighterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeHealthP1) SetHUDHealthP1(HUDHealthP1, HUDMaxHealthP1);
				if (bInitializeHealthP2) SetHUDHealthP2(HUDHealthP2, HUDMaxHealthP2);
			}
		}
	}
}

