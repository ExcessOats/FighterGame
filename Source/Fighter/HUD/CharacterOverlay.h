// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBarP1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthTextP1;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBarP2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthTextP2;

};
