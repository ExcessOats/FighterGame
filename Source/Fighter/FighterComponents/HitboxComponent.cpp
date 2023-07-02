// Fill out your copyright notice in the Description page of Project Settings.


#include "HitboxComponent.h"


UHitboxComponent::UHitboxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HitboxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hitbox"));
	//SetRootComponent(HitboxMesh);

}

void UHitboxComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}

void UHitboxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

