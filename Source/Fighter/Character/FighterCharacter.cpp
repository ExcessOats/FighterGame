// Copyright Epic Games, Inc. All Rights Reserved.

#include "FighterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Fighter/Camera/FightCamera.h"
#include "Fighter/PlayerController/FighterPlayerController.h"
#include "Fighter/FighterComponents/CombatComponent.h"
#include "Fighter/Hitbox/Hitbox.h"
#include "Fighter/Fighter.h"
#include "Components/BoxComponent.h"
#include "Fighter/GameMode/FGGameMode.h"



//////////////////////////////////////////////////////////////////////////
// AFighterCharacter

AFighterCharacter::AFighterCharacter()
{

	GetCapsuleComponent()->InitCapsuleSize(30.f, 88.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Hitbox, ECollisionResponse::ECR_Block);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 

	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->AirControl = 0.f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetupAttachment(GetMesh(), FName("RightHandMeleeSocket"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Pushbox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PushBox"));
	Pushbox->SetupAttachment(GetMesh());
	HurtboxesMap.Add(0, Pushbox);

	HighHurtbox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HighHurtbox"));
	HighHurtbox->SetupAttachment(GetMesh());
	HurtboxesMap.Add(1, HighHurtbox);

	MidHurtbox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MidHurtbox"));
	MidHurtbox->SetupAttachment(GetMesh());
	HurtboxesMap.Add(2, MidHurtbox);

	LowHurtbox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LowHurtbox"));
	LowHurtbox->SetupAttachment(GetMesh());
	HurtboxesMap.Add(3, LowHurtbox);

	ThrowHurtbox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ThrowHurtbox"));
	ThrowHurtbox->SetupAttachment(GetMesh());
	HurtboxesMap.Add(4, ThrowHurtbox);

	for (auto Box : HurtboxesMap)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_Hitbox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_Hitbox, ECollisionResponse::ECR_Overlap);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);			
			Box.Value->SetVisibility(bShowHurtboxes);
			Box.Value->SetRelativeScale3D(FVector(.05f, 1.f, 1.f));
		}
	}
	Pushbox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void AFighterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFighterCharacter::Jump);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFighterCharacter::Move);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AFighterCharacter::CrouchPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AFighterCharacter::CrouchReleased);
		EnhancedInputComponent->BindAction(Attack1Action, ETriggerEvent::Started, this, &AFighterCharacter::Attack1Pressed);
		EnhancedInputComponent->BindAction(Attack2Action, ETriggerEvent::Started, this, &AFighterCharacter::Attack2Pressed);
		EnhancedInputComponent->BindAction(Attack3Action, ETriggerEvent::Started, this, &AFighterCharacter::Attack3Pressed);
		EnhancedInputComponent->BindAction(Attack4Action, ETriggerEvent::Started, this, &AFighterCharacter::Attack4Pressed);
	}
}

void AFighterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFighterCharacter, Health);
	DOREPLIFETIME(AFighterCharacter, bCurrentFlip);
	DOREPLIFETIME(AFighterCharacter, bBackwards);
	DOREPLIFETIME(AFighterCharacter, OtherPlayer);
	DOREPLIFETIME(AFighterCharacter, bStartsLeft);
	DOREPLIFETIME(AFighterCharacter, bDisableMovement);
}

void AFighterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
}

void AFighterCharacter::BeginPlay()
{

	Super::BeginPlay();


	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FighterContext, 0);
		}
	}
	
	CacheHurtboxes();

	FTimerHandle StartTime;
	GetWorldTimerManager().SetTimer(StartTime, this, &AFighterCharacter::CameraSetup, .7f, false); // fix this so they spawn after other player is set in the game mode and not with this delay

	if (HasAuthority())
	{
		
		
	}
	OnTakeAnyDamage.AddDynamic(this, &AFighterCharacter::ReceiveDamage);
	HandleUpdateHealth.AddDynamic(this, &AFighterCharacter::UpdateHUDHealth);
}

void AFighterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OtherPlayer)
	{
		if (GetActorLocation().X > OtherPlayer->GetActorLocation().X)
		{
			bFlipped = true;
			if (bFlipped != bCurrentFlip)
			{
				UE_LOG(LogTemp, Warning, TEXT("Flip"));
				FlipCharacter(bFlipped);
			}
			
		}
		else
		{
			bFlipped = false;
			if (bFlipped != bCurrentFlip)
			{
				UE_LOG(LogTemp, Warning, TEXT("Flip Back"));
				FlipCharacter(bFlipped);
			}
		}
	}
	if (FightCamera && CamPlayer1 && CamPlayer2)
	{
		CameraMovement();
	}
}

void AFighterCharacter::CameraMovement()
{
	const FVector P1Location = CamPlayer1->GetMesh()->GetComponentLocation();
	const FVector P2Location = CamPlayer2->GetMesh()->GetComponentLocation();
	FVector Center = P1Location + P2Location;
	Center.X /= 2.f;
	Center.Y = 0.f;
	Center.Z /= FightCamera->ZRatio;
	Center.Z += FightCamera->ZOffset;
	FightCamera->SetActorLocation(Center);

	float FOVCenter = P1Location.X - P2Location.X;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Distance: %f"), FOVCenter));
	FOVCenter *= FightCamera->FOVRatio;
	CurrentFOV = FMath::Clamp(FMath::Abs(FOVCenter), FightCamera->FOVMin, FightCamera->FOVMax);
	FightCamera->GetFightCamera()->SetFieldOfView(CurrentFOV);
}

void AFighterCharacter::CameraSetup()
{
	UE_LOG(LogTemp, Warning, TEXT("CameraSetup"));
	UWorld* World = GetWorld();
	if (FightCamera != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Camera Destroyed"));
		FightCamera->Destroy();
	}
	if (FightCameraClass)
	{
		FightCamera = World->SpawnActor<AFightCamera>(FightCameraClass, FVector(), FRotator());
		UE_LOG(LogTemp, Warning, TEXT("Camera Spawn"));
		if (bStartsLeft)
		{
			CamPlayer1 = this;
			CamPlayer2 = OtherPlayer;
		}
		else
		{
			CamPlayer1 = OtherPlayer;
			CamPlayer2 = this;
			bCurrentFlip = true;
			bFlipped = true;
		}
		InitializeHealthBars();

		if (FightCamera != nullptr)
		{
			FighterPlayerController = FighterPlayerController == nullptr ? Cast<AFighterPlayerController>(Controller) : FighterPlayerController;
			if (FighterPlayerController)
			{
				UE_LOG(LogTemp, Warning, TEXT("SetView"));
				FighterPlayerController->SetViewTargetWithBlend(FightCamera);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No FightCamera Spawned"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No FightCameraClass set"));
	}
}

void AFighterCharacter::UpdateHUDHealth(float NewHealth, APlayerState* DamagedPlayer)
{
	FighterPlayerController = FighterPlayerController == nullptr ? Cast<AFighterPlayerController>(Controller) : FighterPlayerController;
	AFighterPlayerController* OtherPlayerController = Cast<AFighterPlayerController>(OtherPlayer->GetController());
	if (FighterPlayerController && OtherPlayerController)//
	{
		if (bLeftHealthBar)
		{
			UE_LOG(LogTemp, Warning, TEXT("Update Health P1"));
			FighterPlayerController->SetHUDHealthP1(NewHealth, MaxHealth);
			OtherPlayerController->SetHUDHealthP1(NewHealth, MaxHealth);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Update Health P2"));
			FighterPlayerController->SetHUDHealthP2(NewHealth, MaxHealth);
			OtherPlayerController->SetHUDHealthP2(NewHealth, MaxHealth);
		}
	}

	/*
	FighterPlayerController = FighterPlayerController == nullptr ? Cast<AFighterPlayerController>(Controller) : FighterPlayerController;

	if (FighterPlayerController)
	{
		if (bStartsLeft)
		{
			FighterPlayerController->SetHUDHealthP1(Health, MaxHealth);
		}
		else
		{
			FighterPlayerController->SetHUDHealthP2(Health, MaxHealth);
		}
	}

	if (!HasAuthority())
	{
		ServerUpdateHUDHealth(NewHealth, bLeftHealthBar);
	}
	else
	{
		MulticastUpdateHUDHealth(NewHealth, bLeftHealthBar);
	}
	*/
}

void AFighterCharacter::ServerUpdateHUDHealth_Implementation(float NewHealth, APlayerState* DamagedPlayer)
{
	MulticastUpdateHUDHealth(NewHealth, DamagedPlayer);
}

void AFighterCharacter::MulticastUpdateHUDHealth_Implementation(float NewHealth, APlayerState* DamagedPlayer)
{	
	HandleUpdateHealth.Broadcast(NewHealth, DamagedPlayer);
}

void AFighterCharacter::InitializeHealthBars()
{
	FighterPlayerController = FighterPlayerController == nullptr ? Cast<AFighterPlayerController>(Controller) : FighterPlayerController;
	if (FighterPlayerController)
	{
		if (bStartsLeft)
		{
			FighterPlayerController->SetHUDHealthP1(Health, MaxHealth);
			FighterPlayerController->SetHUDHealthP2(OtherPlayer->Health, OtherPlayer->MaxHealth);
		}
		else
		{
			FighterPlayerController->SetHUDHealthP1(OtherPlayer->Health, OtherPlayer->MaxHealth);
			FighterPlayerController->SetHUDHealthP2(Health, MaxHealth);
		}
	}
}

void AFighterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("ReceiveDamage"));
	Health -= Damage;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);
	PlayHitReactMontage();
	FighterPlayerController = FighterPlayerController == nullptr ? Cast<AFighterPlayerController>(Controller) : FighterPlayerController;
	if (FighterPlayerController)
	{
		APlayerState* PS = FighterPlayerController->PlayerState;
		ServerUpdateHUDHealth(Health, PS);
	}
	
}

void AFighterCharacter::OnRep_Flipped()
{

}

void AFighterCharacter::OnRep_Health(float LastHealth)
{
	//UpdateHUDHealth(Health, bStartsLeft);	
	PlayHitReactMontage();

}

void AFighterCharacter::FlipCharacter(bool bCharacterFlipped)
{
	ServerFlipCharacter(bCharacterFlipped);
}

void AFighterCharacter::ServerFlipCharacter_Implementation(bool bCharacterFlipped)
{
	MulticastFlipCharacter(bCharacterFlipped);
}

void AFighterCharacter::MulticastFlipCharacter_Implementation(bool bCharacterFlipped)
{
	bCurrentFlip = bCharacterFlipped;
	FRotator InitRot = GetActorRotation();
	InitRot.Yaw += 180;
	SetActorRotation(InitRot.Quaternion());
}


void AFighterCharacter::Move(const FInputActionValue& Value)
{
	if (bDisableMovement) return;
	if (Combat->CombatState == ECombatState::ECS_Unoccupied || Combat->CombatState == ECombatState::ECS_Dashing)
	{
		FVector2D MovementVector = Value.Get<FVector2D>();

		if (Controller != nullptr && !bIsCrouched)
		{

			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			const FRotator AimRotation = GetBaseAimRotation();
			const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(GetVelocity());
			const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
			if (-90.f > DeltaRot.Yaw || DeltaRot.Yaw > 90.f)
			{
				if (bFlipped) bBackwards = false;
				else bBackwards = true;
			}
			else
			{
				if (bFlipped) bBackwards = true;
				else bBackwards = false;
			}
			if (!HasAuthority())
			{
				ServerSetBackwards(bBackwards);
			}

			AddMovementInput(RightDirection, MovementVector.X);
		}
	}	
}

void AFighterCharacter::ServerSetBackwards_Implementation(bool bBack)
{
	bBackwards = bBack;
}

void AFighterCharacter::Jump()
{
	if (bDisableMovement) return;
	if (Combat->CombatState == ECombatState::ECS_Unoccupied || Combat->CombatState == ECombatState::ECS_Dashing)
	{
		Super::Jump();
	}
}

void AFighterCharacter::CrouchPressed(const FInputActionValue& Value)
{
	if (bDisableMovement) return;
	if (Combat->CombatState == ECombatState::ECS_Unoccupied || Combat->CombatState == ECombatState::ECS_Dashing)
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			Crouch();
			HighHurtbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ThrowHurtbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ModifyHurtbox(HurtboxCrouchStates);
			if (bShowHurtboxes)
			{
				HighHurtbox->SetVisibility(false);
				ThrowHurtbox->SetVisibility(false);
			}
			if (!HasAuthority())
			{
				ServerHandleCrouch(false, false);
			}
			else
			{
				MulticastHandleCrouch(false, false);
			}
		}
			
	}
}

void AFighterCharacter::CrouchReleased(const FInputActionValue& Value)
{
	UnCrouch();
	HighHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ThrowHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ResetHurtboxes();
	if (bShowHurtboxes)
	{
		HighHurtbox->SetVisibility(true);
		ThrowHurtbox->SetVisibility(true);
	}
	if (!HasAuthority())
	{
		ServerHandleCrouch(true, true);
	}
	else
	{
		MulticastHandleCrouch(true, true);
	}
}

void AFighterCharacter::ServerHandleCrouch_Implementation(bool Collision, bool Display)
{
	MulticastHandleCrouch(Collision, Display);
}

void AFighterCharacter::MulticastHandleCrouch_Implementation(bool Collision, bool Display)
{
	if (!HasAuthority() && IsLocallyControlled()) return;
	if (Collision)
	{
		HighHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ThrowHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		HighHurtbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ThrowHurtbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (bShowHurtboxes)
	{
		HighHurtbox->SetVisibility(Display);
		ThrowHurtbox->SetVisibility(Display);
	}
}

void AFighterCharacter::Attack1Pressed(const FInputActionValue& Value)
{
	if (Combat->CombatState == ECombatState::ECS_Unoccupied || Combat->CombatState == ECombatState::ECS_Dashing)
	{
		Combat->Attack1Pressed(true);
	}
}

void AFighterCharacter::Attack2Pressed(const FInputActionValue& Value)
{
	if (Combat->CombatState == ECombatState::ECS_Unoccupied || Combat->CombatState == ECombatState::ECS_Dashing)
	{
		Combat->Attack2Pressed(true);
	}
}

void AFighterCharacter::Attack3Pressed(const FInputActionValue& Value)
{
	if (Combat->CombatState == ECombatState::ECS_Unoccupied || Combat->CombatState == ECombatState::ECS_Dashing)
	{
		Combat->Attack3Pressed(true);
	}
}

void AFighterCharacter::Attack4Pressed(const FInputActionValue& Value)
{
	if (Combat->CombatState == ECombatState::ECS_Unoccupied || Combat->CombatState == ECombatState::ECS_Dashing)
	{
		Combat->Attack4Pressed(true);
	}
}

void AFighterCharacter::PlayAttack1Montage()
{
	if (Combat == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Attack1Montage && Combat)
	{
		AnimInstance->Montage_Play(Attack1Montage);
	}
}

void AFighterCharacter::PlayAttack2Montage()
{
	if (Combat == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Attack2Montage && Combat)
	{
		AnimInstance->Montage_Play(Attack2Montage);
	}
}

void AFighterCharacter::PlayAttack3Montage()
{
	if (Combat == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Attack3Montage && Combat)
	{
		AnimInstance->Montage_Play(Attack3Montage);
	}
}

void AFighterCharacter::PlayAttack4Montage()
{
	if (Combat == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Attack4Montage && Combat)
	{
		AnimInstance->Montage_Play(Attack4Montage);
	}
}

void AFighterCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage && Combat)
	{
		AnimInstance->Montage_Play(HitReactMontage);
	}
}

void AFighterCharacter::Destroyed()
{
	Super::Destroyed();

	if (FightCamera) FightCamera->Destroy();
}

void AFighterCharacter::CacheHurtboxes()
{
	for (auto Box : HurtboxesMap)
	{
		if (Box.Value)
		{
			Box.Value->SetVisibility(bShowHurtboxes);
			HurtboxDefaultStates.Add(Box.Value->GetRelativeTransform());
			if (Box.Key == 0)
			{
				FTransform Transform;
				Transform.SetLocation(FVector(PushboxCrouchXOffset, 0.f, PushboxCrouchZOffset));
				Transform.SetScale3D(FVector(.05f, PushboxCrouchXScale, PushboxCrouchZScale));
				HurtboxCrouchStates.Add(Transform);
			}
			if (Box.Key == 1)
			{
				FTransform Transform;
				Transform.SetLocation(FVector(HighHurtboxCrouchXOffset, 0.f, HighHurtboxCrouchZOffset));
				Transform.SetScale3D(FVector(.05f, HighHurtboxCrouchXScale, HighHurtboxCrouchZScale));
				HurtboxCrouchStates.Add(Transform);
			}
			if (Box.Key == 2)
			{
				FTransform Transform;
				Transform.SetLocation(FVector(MidHurtboxCrouchXOffset, 0.f, MidHurtboxCrouchZOffset));
				Transform.SetScale3D(FVector(.05f, MidHurtboxCrouchXScale, MidHurtboxCrouchZScale));
				HurtboxCrouchStates.Add(Transform);
			}
			if (Box.Key == 3)
			{
				FTransform Transform;
				Transform.SetLocation(FVector(LowHurtboxCrouchXOffset, 0.f, LowHurtboxCrouchZOffset));
				Transform.SetScale3D(FVector(.05f, LowHurtboxCrouchXScale, LowHurtboxCrouchZScale));
				HurtboxCrouchStates.Add(Transform);
			}
			if (Box.Key == 4)
			{
				FTransform Transform;
				Transform.SetLocation(FVector(ThrowHurtboxCrouchXOffset, 0.f, ThrowHurtboxCrouchZOffset));
				Transform.SetScale3D(FVector(.05f, ThrowHurtboxCrouchXScale, ThrowHurtboxCrouchZScale));
				HurtboxCrouchStates.Add(Transform);
			}
		}
	}
}

void AFighterCharacter::ResetHurtboxes()
{
	for (auto Box : HurtboxesMap)
	{
		if (Box.Value)
		{
			Box.Value->SetRelativeTransform(HurtboxDefaultStates[Box.Key]);
		}
	}
	if (!HasAuthority())
	{
		ServerResetHurtboxes();
	}
	else
	{
		MulticastResetHurtboxes();
	}
}

void AFighterCharacter::ModifyHurtbox(const TArray<FTransform>& HurtboxStateMap)
{
	for (auto Box : HurtboxesMap)
	{
		if (Box.Value)
		{
			Box.Value->SetRelativeTransform(HurtboxStateMap[Box.Key]);
		}
	}
	if (!HasAuthority())
	{
		ServerModifyHurtbox(HurtboxStateMap);
	}
	else
	{
		MulticastModifyHurtbox(HurtboxStateMap);
	}
}

void AFighterCharacter::ServerResetHurtboxes_Implementation()
{
	MulticastResetHurtboxes();
}

void AFighterCharacter::ServerModifyHurtbox_Implementation(const TArray<FTransform>& HurtboxStateMap)
{
	MulticastModifyHurtbox(HurtboxStateMap);
}

void AFighterCharacter::MulticastResetHurtboxes_Implementation()
{
	if (!HasAuthority() && IsLocallyControlled()) return;
	for (auto Box : HurtboxesMap)
	{
		if (Box.Value)
		{
			Box.Value->SetRelativeTransform(HurtboxDefaultStates[Box.Key]);
		}
	}
}

void AFighterCharacter::MulticastModifyHurtbox_Implementation(const TArray<FTransform>& HurtboxStateMap)
{
	if (!HasAuthority() && IsLocallyControlled()) return;
	for (auto Box : HurtboxesMap)
	{
		if (Box.Value)
		{
			Box.Value->SetRelativeTransform(HurtboxStateMap[Box.Key]);
		}
	}
}

ECombatState AFighterCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

FCollisionQueryParams AFighterCharacter::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams Params;

	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);

	return Params;
}


