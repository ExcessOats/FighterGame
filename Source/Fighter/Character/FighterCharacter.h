// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Fighter/FighterTypes/CombatState.h"
#include "FighterCharacter.generated.h"

class UInputMappingContext;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterFlipped, bool, bCharacterFlipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateHealth, float, NewHealth, APlayerState*, DamagedPlayer);

UENUM(BlueprintType)
enum class EDirectionalInput : uint8
{
	DI_Neutral		UMETA(DisplayName = "Neutral"),
	DI_Forward		UMETA(DisplayName = "Forward"),
	DI_Backward		UMETA(DisplayName = "Backward")
};

UCLASS(config=Game)
class AFighterCharacter : public ACharacter
{
	GENERATED_BODY()


public:

	AFighterCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	void CameraSetup();

	UFUNCTION()
	void UpdateHUDHealth(float NewHealth, APlayerState* DamagedPlayer);

	UFUNCTION(Server, Reliable)
	void ServerUpdateHUDHealth(float NewHealth, APlayerState* DamagedPlayer);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateHUDHealth(float NewHealth, APlayerState* DamagedPlayer);

	UFUNCTION()
	void FlipCharacter(bool bCharacterFlipped);

	UFUNCTION(Server, Reliable)
	void ServerFlipCharacter(bool bCharacterFlipped);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFlipCharacter(bool bCharacterFlipped);


	/*
		PlayMontages
	*/

	void PlayAttack1Montage();
	void PlayAttack2Montage();
	void PlayAttack3Montage();
	void PlayAttack4Montage();
	void PlayHitReactMontage();

	
	UPROPERTY(VisibleAnywhere)
	bool bFlipped = false;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Flipped) //
	bool bCurrentFlip = false;

	UPROPERTY(Replicated)
	bool bBackwards = false;

	UPROPERTY(Replicated)
	bool bStartsLeft = false;

	UPROPERTY(Replicated)
	bool bDisableMovement = false;

	UPROPERTY(VisibleAnywhere, Replicated)
	AFighterCharacter* OtherPlayer;

	UPROPERTY()
	FOnCharacterFlipped CharacterFlipped;

	UPROPERTY()
	FOnUpdateHealth HandleUpdateHealth;

	UPROPERTY()
	TMap<int32, class UStaticMeshComponent*> HurtboxesMap;

	FCollisionQueryParams GetIgnoreCharacterParams() const;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* FighterContext;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Attack1Action;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Attack2Action;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Attack3Action;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Attack4Action;

	void Move(const FInputActionValue& Value);
	void MoveReleased();
	virtual void Jump() override;
	void CrouchPressed(const FInputActionValue& Value);
	void CrouchReleased(const FInputActionValue& Value);
	void Attack1Pressed(const FInputActionValue& Value);
	void Attack2Pressed(const FInputActionValue& Value);
	void Attack3Pressed(const FInputActionValue& Value);
	void Attack4Pressed(const FInputActionValue& Value);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	UFUNCTION(Server, Reliable)
	void ServerSetBackwards(bool bBack);
	
	UFUNCTION(Server, Reliable)
	void ServerHandleCrouch(bool Collision, bool Display);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleCrouch(bool Collision, bool Display);


private:

	UPROPERTY(Replicated)
	class AFighterPlayerController* FighterPlayerController;

	UPROPERTY()
	class AFGGameMode* FGGameMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EDirectionalInput Direction = EDirectionalInput::DI_Neutral;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AFightCamera> FightCameraClass;

	UPROPERTY(VisibleAnywhere)
	AFightCamera* FightCamera;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* WeaponMesh;

	void CameraMovement();

	float CurrentFOV;

	UPROPERTY(VisibleAnywhere, Category = "Player References")
	AFighterCharacter* CamPlayer1;

	UPROPERTY(VisibleAnywhere, Category = "Player References")
	AFighterCharacter* CamPlayer2;

	UFUNCTION()
	void OnRep_Flipped();

	/*
		Player Health
	*/

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	UPROPERTY(VisibleAnywhere, Category = "Player Stats")
	float OtherPlayerMaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Player Stats")
	float OtherPlayerHealth = 100.f;

	void InitializeHealthBars();

	/*
		Montages
	*/

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* Attack1Montage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* Attack2Montage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* Attack3Montage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* Attack4Montage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	/*
		Hurtbox
	*/

	UPROPERTY(EditAnywhere, Category = Pushbox)
	UStaticMeshComponent* Pushbox;

	UPROPERTY(EditAnywhere, Category = HighHurtbox)
	UStaticMeshComponent* HighHurtbox;

	UPROPERTY(EditAnywhere, Category = MidHurtbox)
	UStaticMeshComponent* MidHurtbox;

	UPROPERTY(EditAnywhere, Category = LowHurtbox)
	UStaticMeshComponent* LowHurtbox;

	UPROPERTY(EditAnywhere, Category = ThrowHurtbox)
	UStaticMeshComponent* ThrowHurtbox;

	UPROPERTY(EditAnywhere, Category = Hurtbox)
	bool bShowHurtboxes;

	void CacheHurtboxes();

	void ResetHurtboxes();

	void ModifyHurtbox(const TArray<FTransform>& HurtboxStateMap);

	UFUNCTION(Server, Reliable)
	void ServerResetHurtboxes();

	UFUNCTION(Server, Reliable)
	void ServerModifyHurtbox(const TArray<FTransform>& HurtboxStateMap);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastResetHurtboxes();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastModifyHurtbox(const TArray<FTransform>& HurtboxStateMap);

	//TMap<FName, FTransform> HurtboxDefaultStates;

	//TMap<FName, FTransform> HurtboxCrouchStates;

	TArray<FTransform> HurtboxDefaultStates;

	TArray<FTransform> HurtboxCrouchStates;

	//Pushbox

	UPROPERTY(EditAnywhere, Category = Pushbox)
	float PushboxCrouchXOffset;

	UPROPERTY(EditAnywhere, Category = Pushbox)
	float PushboxCrouchZOffset;

	UPROPERTY(EditAnywhere, Category = Pushbox)
	float PushboxCrouchXScale = 1.f;

	UPROPERTY(EditAnywhere, Category = Pushbox)
	float PushboxCrouchZScale = 1.f;

	// High Hurtbox

	UPROPERTY(EditAnywhere, Category = HighHurtbox)
	float HighHurtboxCrouchXOffset;

	UPROPERTY(EditAnywhere, Category = HighHurtbox)
	float HighHurtboxCrouchZOffset;

	UPROPERTY(EditAnywhere, Category = HighHurtbox)
	float HighHurtboxCrouchXScale = 1.f;

	UPROPERTY(EditAnywhere, Category = HighHurtbox)
	float HighHurtboxCrouchZScale = 1.f;

	// Mid Hurtbox

	UPROPERTY(EditAnywhere, Category = MidHurtbox)
	float MidHurtboxCrouchXOffset;

	UPROPERTY(EditAnywhere, Category = MidHurtbox)
	float MidHurtboxCrouchZOffset;

	UPROPERTY(EditAnywhere, Category = MidHurtbox)
	float MidHurtboxCrouchXScale = 1.f;

	UPROPERTY(EditAnywhere, Category = MidHurtbox)
	float MidHurtboxCrouchZScale = 1.f;

	// Low Hurtbox

	UPROPERTY(EditAnywhere, Category = LowHurtbox)
	float LowHurtboxCrouchXOffset;

	UPROPERTY(EditAnywhere, Category = LowHurtbox)
	float LowHurtboxCrouchZOffset;

	UPROPERTY(EditAnywhere, Category = LowHurtbox)
	float LowHurtboxCrouchXScale = 1.f;

	UPROPERTY(EditAnywhere, Category = LowHurtbox)
	float LowHurtboxCrouchZScale = 1.f;

	// Throw Hurtbox

	UPROPERTY(EditAnywhere, Category = ThrowHurtbox)
	float ThrowHurtboxCrouchXOffset;

	UPROPERTY(EditAnywhere, Category = ThrowHurtbox)
	float ThrowHurtboxCrouchZOffset;

	UPROPERTY(EditAnywhere, Category = ThrowHurtbox)
	float ThrowHurtboxCrouchXScale = 1.f;

	UPROPERTY(EditAnywhere, Category = ThrowHurtbox)
	float ThrowHurtboxCrouchZScale = 1.f;

public:

	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE UInputMappingContext* GetFighterContext() const { return FighterContext; }
	FORCEINLINE AFightCamera* GetFightCamera() const { return FightCamera; }
	ECombatState GetCombatState() const;
	FORCEINLINE float GetHealth () const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

};

