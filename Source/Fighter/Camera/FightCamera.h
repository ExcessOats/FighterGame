// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FightCamera.generated.h"

UCLASS()
class FIGHTER_API AFightCamera : public AActor
{
	GENERATED_BODY()
	
public:	

	AFightCamera();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void GetPlayers(class AFighterCharacter* Player1, AFighterCharacter* Player2);

	UPROPERTY(EditAnywhere, Category = Camera)
	float ZOffset = 0.f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float ZRatio = 1.5f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float FOVRatio = .15f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float FOVMin = 60.f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float FOVMax = 80.f;

	//UPROPERTY(ReplicatedUsing = OnRep_Camera)
	float CurrentFOV;

	UPROPERTY(VisibleAnywhere, Category = "Player References")
	AFighterCharacter* CamPlayer1;

	UPROPERTY(VisibleAnywhere, Category = "Player References")
	AFighterCharacter* CamPlayer2;


protected:

	virtual void BeginPlay() override;

private:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FightCamera;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* CameraTracker;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BoundaryLeft;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BoundaryRight;

	UPROPERTY(EditAnywhere)
	float MaxPlayerDistance = 750;
	

	//UFUNCTION()
	//void OnRep_Camera();

public:	

	FORCEINLINE class UCameraComponent* GetFightCamera() const { return FightCamera; }

};
