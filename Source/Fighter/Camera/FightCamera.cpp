// Fill out your copyright notice in the Description page of Project Settings.


#include "FightCamera.h"
#include "Fighter/GameMode/FGGameMode.h"
#include "Fighter/Character/FighterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


AFightCamera::AFightCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	//bReplicates = true;
	//SetReplicateMovement(true);

	CameraTracker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraTracker"));
	SetRootComponent(CameraTracker);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(CameraTracker);
	CameraBoom->TargetArmLength = 500.0f; 

	FightCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FightCamera"));
	FightCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	BoundaryLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoundaryLeft"));
	BoundaryLeft->SetupAttachment(CameraTracker);
	BoundaryLeft->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoundaryLeft->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	BoundaryLeft->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoundaryLeft->SetRelativeScale3D(FVector(5.f, 1.f, 5.f));
	BoundaryLeft->bHiddenInGame = true;

	BoundaryRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoundaryRight"));
	BoundaryRight->SetupAttachment(CameraTracker);
	BoundaryRight->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoundaryRight->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	BoundaryRight->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoundaryRight->SetRelativeScale3D(FVector(5.f, 1.f, 5.f));
	BoundaryRight->bHiddenInGame = true;
}


void AFightCamera::BeginPlay()
{
	Super::BeginPlay();

	BoundaryLeft->SetRelativeLocation(FVector(0.f, -MaxPlayerDistance, 0.f));
	BoundaryRight->SetRelativeLocation(FVector(0.f, MaxPlayerDistance, 0.f));
}


void AFightCamera::GetPlayers(AFighterCharacter* Player1, AFighterCharacter* Player2)
{
	UE_LOG(LogTemp, Warning, TEXT("CamPlayers"));
	CamPlayer1 = Player1;
	CamPlayer2 = Player2;
}

void AFightCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// TODO for camera edges, max distance is 740
}

void AFightCamera::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AFightCamera, CurrentFOV);
}

