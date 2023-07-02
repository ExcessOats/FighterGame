// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitboxComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTER_API UHitboxComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UHitboxComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	
	virtual void BeginPlay() override;

private:
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* HitboxMesh;

public:	

	

		
};
