// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Fighter/FighterTypes/HitboxData.h"
#include "Hitbox.generated.h"


UCLASS()
class FIGHTER_API AHitbox : public AActor
{
	GENERATED_BODY()
	
public:	

	AHitbox();
	virtual void Tick(float DeltaTime) override;
	void SetHitboxMaterial(EHitboxType Type);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void CheckOverlappingActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
	FHitboxData HitboxData;

	UPROPERTY() 
	EHitboxType HitboxType;

	float Damage;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnOverlapBegin(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBox;

private:

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* HitboxMesh;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* ProximityMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* StrikeMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* HurtboxMaterial;

	UPROPERTY()
	EHitboxHeight HitboxHeight;

	UPROPERTY(ReplicatedUsing = OnRep_Hits, VisibleAnywhere)
	int32 Hits = 0;

	UPROPERTY(EditAnywhere)
	int32 MaxHits = 1;

	UFUNCTION()
	void OnRep_Hits();

	UPROPERTY(EditAnywhere)
	bool bShowHitbox = false;
	

};
