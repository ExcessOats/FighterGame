// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Fighter/FighterTypes/CombatState.h"
#include "Engine/DataTable.h"
#include "Fighter/FighterTypes/HitboxData.h"
#include "Fighter/Hitbox/Hitbox.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	friend class AFighterCharacter;
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void SpawnProxyHitbox(FHitboxData Row);

	UFUNCTION(BlueprintCallable)
	void SpawnActiveHitbox(FHitboxData Row);

	UFUNCTION(Server, Reliable)
	void ServerSpawnProxyHitbox(FVector_NetQuantize Location, FVector_NetQuantize Scale, EHitboxType HBType, bool bAttach);

	UFUNCTION(Server, Reliable)
	void ServerSpawnActiveHitbox(FVector_NetQuantize Location, FVector_NetQuantize Scale, EHitboxType HBType, bool bAttach, float Damage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnProxyHitbox(FVector_NetQuantize Location, FVector_NetQuantize Scale, EHitboxType HBType, bool bAttach);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnActiveHitbox(FVector_NetQuantize Location, FVector_NetQuantize Scale, EHitboxType HBType, bool bAttach, float Damage);

	UFUNCTION(BlueprintCallable)
	void AttackEnded();

	UFUNCTION(Server, Reliable)
	void ServerAttackEnded();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttackEnded();
	

protected:

	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Hitbox)
	class UDataTable* HitboxDT;

private:

	class AFighterCharacter* Character;

	UPROPERTY(EditAnywhere)
	bool bShowProxyHitbox = false;

	UPROPERTY(EditAnywhere)
	bool bShowActiveHitbox = false;

	UPROPERTY(ReplicatedUsing = OnRep_ProxyHitbox) //
	class AHitbox* ProxyHitbox;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveHitbox) //
	AHitbox* ActiveHitbox;

	UFUNCTION()
	void OnRep_ProxyHitbox();

	UFUNCTION()
	void OnRep_ActiveHitbox();

	EHitboxType ProxyHBType;
	EHitboxType ActiveHBType;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHitbox> HitboxClass;

	TArray<AHitbox*> ProxyHitboxes;

	TArray<AHitbox*> ActiveHitboxes;

	bool CanAttack();

	bool bCanAttack = true;

	bool bAttack1Pressed = false;
	bool bAttack2Pressed = false;
	bool bAttack3Pressed = false;
	bool bAttack4Pressed = false;

	void Attack1Pressed(bool bPressed);
	void Attack2Pressed(bool bPressed);
	void Attack3Pressed(bool bPressed);
	void Attack4Pressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerAttack1();

	UFUNCTION(Server, Reliable)
	void ServerAttack2();

	UFUNCTION(Server, Reliable)
	void ServerAttack3();

	UFUNCTION(Server, Reliable)
	void ServerAttack4();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack1();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack2();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack3();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack4();

public:	

	
		
};
