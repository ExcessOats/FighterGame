// Fill out your copyright notice in the Description page of Project Settings.


#include "Hitbox.h"
#include "Fighter/Fighter.h"
#include "Net/UnrealNetwork.h"
#include "Fighter/Character/FighterCharacter.h"
#include "Fighter/PlayerController/FighterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"

AHitbox::AHitbox()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	HitboxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hitbox"));
	SetRootComponent(HitboxMesh);

	HitboxMesh->SetCollisionObjectType(ECC_Hitbox);
	HitboxMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HitboxMesh->SetCollisionResponseToChannel(ECC_Hitbox, ECollisionResponse::ECR_Overlap);
	HitboxMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AHitbox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHitbox, Hits);
}

void AHitbox::BeginPlay()
{
	Super::BeginPlay();

	HitboxMesh->SetHiddenInGame(!bShowHitbox);

	if (HasAuthority())
	{
		HitboxMesh->OnComponentBeginOverlap.AddDynamic(this, &AHitbox::OnOverlapBegin);
	}
	
}

void AHitbox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHitbox::OnRep_Hits()
{
	
}

void AHitbox::CheckOverlappingActors()
{
	TArray<AActor*> HurtboxArray;
	HitboxMesh->GetOverlappingActors(HurtboxArray, AFighterCharacter::StaticClass()); //
	if (HurtboxArray.Num() > 0)
	{
		for (int i = 0; i < HurtboxArray.Num(); i++)
		{

			AFighterCharacter* VictimCharacter = Cast<AFighterCharacter>(HurtboxArray[i]);
			if (VictimCharacter)
			{
				AFighterCharacter* OwnerCharacter = Cast<AFighterCharacter>(GetOwner());

				if (OwnerCharacter)
				{
					AFighterPlayerController* OwnerController = Cast<AFighterPlayerController>(OwnerCharacter->Controller);
					if (OwnerController && HasAuthority())
					{
						if (VictimCharacter != GetOwner() && HasAuthority())
						{
							Hits += 1;
							UGameplayStatics::ApplyDamage(VictimCharacter, Damage, OwnerController, this, UDamageType::StaticClass());
							if (Hits >= MaxHits)
							{	
								HitboxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
								HitboxMesh->OnComponentBeginOverlap.RemoveDynamic(this, &AHitbox::OnOverlapBegin);
							}
							return;
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("Hit Self"));
						}
					}
				}
			}
		}
	}
}

/*
void AHitbox::OnRep_HitboxType()
{
	switch (HitboxType)
	{
	case EHitboxType::EHT_Proximity:
		HitboxMesh->SetMaterial(0, ProximityMaterial);
		break;
	case EHitboxType::EHT_Strike:
		HitboxMesh->SetMaterial(0, StrikeMaterial);
		break;
	case EHitboxType::EHT_Hurtbox:
		HitboxMesh->SetMaterial(0, HurtboxMaterial);
		break;
	}
}
*/

void AHitbox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HitboxType == EHitboxType::EHT_Proximity) return;
	if (OtherActor == GetOwner()) return;
	//if (HitboxType != EHitboxType::EHT_Strike) return;
	AFighterCharacter* OwnerCharacter = Cast<AFighterCharacter>(GetOwner());
	AFighterCharacter* VictimCharacter = Cast<AFighterCharacter>(OtherActor);
	if (VictimCharacter)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Victim"));
	}
	if (OwnerCharacter && VictimCharacter) //&& VictimCharacter != OwnerCharacter
	{
		
		AFighterPlayerController* OwnerController = Cast<AFighterPlayerController>(OwnerCharacter->Controller);
		if (OwnerController && HasAuthority())
		{
			Hits += 1;
			UGameplayStatics::ApplyDamage(VictimCharacter, Damage, OwnerController, this, UDamageType::StaticClass());
			if (Hits >= MaxHits)
			{
				HitboxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				HitboxMesh->OnComponentBeginOverlap.RemoveDynamic(this, &AHitbox::OnOverlapBegin);
			}
			return;
			
		}
	}
	    
	// Eg. from Blaster with SSR
	/*
	* ABlasterCharacter* OwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		ABlasterPlayerController* OwnerController = Cast<ABlasterPlayerController>(OwnerCharacter->Controller);
		if (OwnerController && HasAuthority())
		{
			if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
			{
				const float DamageToCause = Hit.BoneName.ToString() == FString("head") ? HeadshotDamage : Damage;

				UGameplayStatics::ApplyDamage(OtherActor, DamageToCause, OwnerController, this, UDamageType::StaticClass());
				Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
				return;
			}
			ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(OtherActor);
			if (bUseServerSideRewind && OwnerCharacter->GetLagCompensation() && OwnerCharacter->IsLocallyControlled() && HitCharacter)
			{
				OwnerCharacter->GetLagCompensation()->ProjectileServerScoreRequest(
					HitCharacter, 
					TraceStart, 
					InitialVelocity, 
					OwnerController->GetServerTime() - OwnerController->SingleTripTime
				);
			}	
		}
	}
	*/
}


void AHitbox::SetHitboxMaterial(EHitboxType Type)
{
	if (Type == EHitboxType::EHT_MAX) return;
	if (this == nullptr) return;

	HitboxType = Type;
	switch (HitboxType)
	{
	case EHitboxType::EHT_Proximity:
		if (ProximityMaterial == nullptr) return;
		HitboxMesh->SetMaterial(0, ProximityMaterial);
		break;
	case EHitboxType::EHT_Strike:
		if (StrikeMaterial == nullptr) return;
		HitboxMesh->SetMaterial(0, StrikeMaterial);
		break;
	case EHitboxType::EHT_Hurtbox:
		if (HurtboxMaterial == nullptr) return;
		HitboxMesh->SetMaterial(0, HurtboxMaterial);
		break;
	}
		
}

