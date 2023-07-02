// Fill out your copyright notice in the Description page of Project Settings.


#include "FGGameMode.h"
#include "Fighter/PlayerController/FighterPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Fighter/Character/FighterCharacter.h"
#include "Fighter/Camera/FightCamera.h"
#include "TimerManager.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"

void AFGGameMode::BeginPlay()
{
	Super::BeginPlay();

	//FTimerHandle StartTime;
	//GetWorldTimerManager().SetTimer(StartTime, this, &AFGGameMode::SpawnFighters, 2.f, false);
	SpawnFighters();
}

void AFGGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void AFGGameMode::SpawnFighters()
{
	if (PCArray.Num() == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("One Player"));
		UWorld* World = GetWorld();
		P1Controller = PCArray[0];
		SpawnPlayer(P1Controller);
		FindPlayerStarts();
		FVector Location = P2Start.GetLocation();
		FRotator Rotation = P2Start.GetRotation().Rotator();
		Player2 = World->SpawnActor<AFighterCharacter>(DefaultCharacterClassTemp, Location, Rotation);
	}
	else 
	{
		if (Player1)
		{
			Player1->Destroy();
		}
		if (Player2)
		{
			Player2->Destroy();
		}
		P1Controller = PCArray[0];
		P2Controller = PCArray[1];
		SpawnPlayer(P1Controller);
		SpawnPlayer(P2Controller);
	}
	Player1->bStartsLeft = true;
	Player1->OtherPlayer = Player2;
	Player2->OtherPlayer = Player1;
	//Player2->bFlipped = true;
	UE_LOG(LogTemp, Warning, TEXT("EndSpawn"));
}

void AFGGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

}

void AFGGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	AFighterPlayerController* LeavingPlayer = Cast<AFighterPlayerController>(Exiting);
	PCArray.Remove(LeavingPlayer);
}

void AFGGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	AFighterPlayerController* JoiningPlayer = Cast<AFighterPlayerController>(NewPlayer);

	PCArray.AddUnique(JoiningPlayer);
	if (PCArray.Num() == 2)
	{
		SpawnFighters();
	}
}

void AFGGameMode::FindPlayerStarts()
{
	UWorld* World = GetWorld();
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts); 
	for (int i = 0; i < PlayerStarts.Num(); i++) // Loop through PlayerStarts
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(PlayerStarts[i]);
		if (PlayerStart)
		{
			FName Tag = PlayerStart->PlayerStartTag;
			FString TagString = Tag.ToString();
			int32 Tagint = FCString::Atoi(*TagString);

			if (Tagint == 0)
			{
				P1Start = PlayerStart->GetActorTransform(); // PlayerStart Tagged with 0 will be for the first player
			}
			if (Tagint == 1)
			{
				P2Start = PlayerStart->GetActorTransform();
			}
		}
	}
}

void AFGGameMode::SpawnPlayer(AFighterPlayerController* SpawnedPlayer)
{
	if (SpawnedPlayer->GetPawn())
	{
		SpawnedPlayer->GetPawn()->Destroy(); // Destroy already existing pawn if it exists so we can spawn our own
	}
	UWorld* World = GetWorld();
	FindPlayerStarts();
	if (SpawnedPlayer == P1Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player1"));
		FVector Location = P1Start.GetLocation();
		FRotator Rotation = P1Start.GetRotation().Rotator();
		Player1 = World->SpawnActor<AFighterCharacter>(DefaultCharacterClassTemp, Location, Rotation);
		SpawnedPlayer->Possess(Player1);
		return;
	}
	else if (SpawnedPlayer == P2Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player2"));
		FVector Location = P2Start.GetLocation();
		FRotator Rotation = P2Start.GetRotation().Rotator();
		Player2 = World->SpawnActor<AFighterCharacter>(DefaultCharacterClassTemp, Location, Rotation);
		SpawnedPlayer->Possess(Player2);
		return;
	}
	
}



