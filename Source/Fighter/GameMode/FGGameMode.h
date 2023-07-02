// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FGGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTER_API AFGGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void FindPlayerStarts();
	virtual void SpawnPlayer(class AFighterPlayerController* SpawnedPlayer);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AFighterCharacter> DefaultCharacterClassTemp; // later this will be set by character selection

	UPROPERTY(VisibleAnywhere, Category = "Player References")
	AFighterCharacter* Player1;

	UPROPERTY(VisibleAnywhere, Category = "Player References")
	AFighterCharacter* Player2;

	TArray<AFighterPlayerController*> PCArray;

	UPROPERTY(VisibleAnywhere, Category = "Player References")
	AFighterPlayerController* P1Controller;

	UPROPERTY(VisibleAnywhere, Category = "Player References")
	AFighterPlayerController* P2Controller;

	FTransform P1Start;

	FTransform P2Start;

protected:

	virtual void BeginPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

private:


	UFUNCTION()
	void SpawnFighters();

};
