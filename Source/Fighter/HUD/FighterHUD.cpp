// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"

void AFighterHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void AFighterHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AFighterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

