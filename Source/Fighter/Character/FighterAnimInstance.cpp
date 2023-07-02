// Fill out your copyright notice in the Description page of Project Settings.FighterCharacter


#include "FighterAnimInstance.h"
#include "FighterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UFighterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	FighterCharacter = Cast<AFighterCharacter>(TryGetPawnOwner());
}

void UFighterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (FighterCharacter == nullptr)
	{
		FighterCharacter = Cast<AFighterCharacter>(TryGetPawnOwner());
	}
	if (FighterCharacter == nullptr) return;

	FVector Velocity = FighterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = FighterCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = FighterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bIsCrouched = FighterCharacter->bIsCrouched;
	bBackwards = FighterCharacter->bBackwards;
	bFlipped = FighterCharacter->bFlipped;

	FRotator AimRotation = FighterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(FighterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	YawOffset = DeltaRot.Yaw;


}
