// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FighterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTER_API AFighterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	void SetHUDHealthP1(float Health, float MaxHealth);
	void SetHUDHealthP2(float Health, float MaxHealth);
	void PollInit();

protected:

	virtual void BeginPlay() override;

private:

	class AFighterHUD* FighterHUD;

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	bool bInitializeHealthP1 = false;
	bool bInitializeHealthP2 = false;

	float HUDHealthP1;
	float HUDMaxHealthP1;
	float HUDHealthP2;
	float HUDMaxHealthP2;

};
