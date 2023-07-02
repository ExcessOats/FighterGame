// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FighterHUD.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTER_API AFighterHUD : public AHUD
{
	GENERATED_BODY()
	

public:

	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	void AddCharacterOverlay();

protected:

	virtual void BeginPlay() override;

};
