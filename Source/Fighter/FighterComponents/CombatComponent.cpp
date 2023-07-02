// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Fighter/Character/FighterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Fighter/Hitbox/Hitbox.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME(UCombatComponent, ProxyHitbox);
	DOREPLIFETIME(UCombatComponent, ActiveHitbox);
}

void UCombatComponent::SpawnProxyHitbox(FHitboxData Row)
{
	ProxyHBType = Row.HitboxType;
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	FVector_NetQuantize Location = Row.Location;
	FVector_NetQuantize Scale = Row.Scale;
	//EHitboxType HBType = Row.HitboxType;
	ServerSpawnProxyHitbox(Location, Scale, ProxyHBType, Row.bShouldAttachToCharacter);
}

void UCombatComponent::SpawnActiveHitbox(FHitboxData Row)
{
	ActiveHBType = Row.HitboxType;
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	FVector_NetQuantize Location = Row.Location;
	FVector_NetQuantize Scale = Row.Scale;
	float Damage = Row.Damage;
	//EHitboxType HBType = Row.HitboxType;
	ServerSpawnActiveHitbox(Location, Scale, ActiveHBType, Row.bShouldAttachToCharacter, Damage);
}

void UCombatComponent::ServerSpawnProxyHitbox_Implementation(FVector_NetQuantize Location, FVector_NetQuantize Scale, EHitboxType HBType, bool bAttach)
{
	MulticastSpawnProxyHitbox(Location, Scale, HBType, bAttach);
}

void UCombatComponent::ServerSpawnActiveHitbox_Implementation(FVector_NetQuantize Location, FVector_NetQuantize Scale, EHitboxType HBType, bool bAttach, float Damage)
{
	MulticastSpawnActiveHitbox(Location, Scale, HBType, bAttach, Damage);
}

void UCombatComponent::MulticastSpawnProxyHitbox_Implementation(FVector_NetQuantize Location, FVector_NetQuantize Scale, EHitboxType HBType, bool bAttach)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	if (Character)
	{
		UWorld* World = GetWorld();
		const FVector CharLocation = Character->GetActorLocation();
		//ProxyHBType = HBType;
		FVector SpawnLocation;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Character;

		AHitbox* SpawnedHitbox = nullptr;
		if (Character->bCurrentFlip)
		{
			SpawnLocation.X = CharLocation.X - Location.X;
		}
		else
		{
			SpawnLocation.X = CharLocation.X + Location.X;
		}
		SpawnLocation.Y = CharLocation.Y + Location.Y;
		SpawnLocation.Z = CharLocation.Z + Location.Z;

		if (HitboxClass)
		{
			ProxyHitbox = World->SpawnActor<AHitbox>(HitboxClass, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);
			ProxyHitboxes.Add(ProxyHitbox);
			ProxyHitbox->HitboxType = HBType;
			ProxyHitbox->SetActorScale3D(Scale);
			ProxyHitbox->SetHitboxMaterial(HBType);
			if (bAttach)
			{
				ProxyHitbox->AttachToActor(Character, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HitboxClass not set"));
		}
	}
}

void UCombatComponent::MulticastSpawnActiveHitbox_Implementation(FVector_NetQuantize Location, FVector_NetQuantize Scale, EHitboxType HBType, bool bAttach, float Damage)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	if (Character)
	{
		UWorld* World = GetWorld();
		const FVector CharLocation = Character->GetActorLocation();
		//ActiveHBType = HBType;
		FVector SpawnLocation;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Character;

		AHitbox* SpawnedHitbox = nullptr;
		if (Character->bCurrentFlip)
		{
			SpawnLocation.X = CharLocation.X - Location.X;
		}
		else
		{
			SpawnLocation.X = CharLocation.X + Location.X;
		}
		SpawnLocation.Y = CharLocation.Y + Location.Y;
		SpawnLocation.Z = CharLocation.Z + Location.Z;

		if (HitboxClass)
		{
			ActiveHitbox = World->SpawnActor<AHitbox>(HitboxClass, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);
			ActiveHitboxes.Add(ActiveHitbox);
			ActiveHitbox->HitboxType = HBType;
			ActiveHitbox->SetActorScale3D(Scale);
			ActiveHitbox->SetHitboxMaterial(HBType);
			ActiveHitbox->Damage = Damage;
			if (bAttach)
			{
				ActiveHitbox->AttachToActor(Character, FAttachmentTransformRules::KeepWorldTransform);
			}
			ActiveHitbox->CheckOverlappingActors();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HitboxClass not set"));
		}
	}

	for (auto Box : ProxyHitboxes)
	{
		Box->Destroy();
	}
	ProxyHitboxes.Empty();
}

void UCombatComponent::AttackEnded()
{
	ServerAttackEnded();
}

void UCombatComponent::ServerAttackEnded_Implementation()
{
	MulticastAttackEnded();
}

void UCombatComponent::MulticastAttackEnded_Implementation()
{
	Character->bDisableMovement = false;
	bCanAttack = true;
	for (auto Box : ProxyHitboxes)
	{
		Box->Destroy();
	}
	ProxyHitboxes.Empty();

	for (auto Box : ActiveHitboxes)
	{
		Box->Destroy();
	}
	ActiveHitboxes.Empty();
}

void UCombatComponent::OnRep_ProxyHitbox()
{
	ProxyHitbox->SetHitboxMaterial(ProxyHBType);
}

void UCombatComponent::OnRep_ActiveHitbox()
{
	ActiveHitbox->SetHitboxMaterial(ActiveHBType);
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Unoccupied:
		if (bAttack1Pressed)
		{
			//Attack1Pressed(bAttack1Pressed);
		}
		break;
/* Reference from Melee in blaster
	case ECombatState::ECS_Melee:
		if (MeleeWeapon->MeleeHitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this,
				MeleeWeapon->MeleeHitSound,
				Character->GetActorLocation()
			);
		}
		break;
		*/
		//RightHandMeleeSocket
	}
}


bool UCombatComponent::CanAttack()
{
	return false;
}

void UCombatComponent::Attack1Pressed(bool bPressed)
{
	bAttack1Pressed = bPressed;
	UE_LOG(LogTemp, Warning, TEXT("Attack 1"));
	if (bAttack1Pressed && bCanAttack)
	{
		bCanAttack = false;
		if (Character)
		{
			Character->bDisableMovement = true;
			Character->PlayAttack1Montage();
		}
		ServerAttack1();
	}
}

void UCombatComponent::Attack2Pressed(bool bPressed)
{
	bAttack2Pressed = bPressed;
	UE_LOG(LogTemp, Warning, TEXT("Attack 2"));
	if (bAttack2Pressed && bCanAttack)
	{
		bCanAttack = false;
		if (Character)
		{
			Character->bDisableMovement = true;
			Character->PlayAttack2Montage();
		}
		ServerAttack2();
	}
}

void UCombatComponent::Attack3Pressed(bool bPressed)
{
	bAttack3Pressed = bPressed;
	UE_LOG(LogTemp, Warning, TEXT("Attack 3"));
	if (bAttack3Pressed && bCanAttack)
	{
		bCanAttack = false;
		if (Character)
		{
			Character->bDisableMovement = true;
			Character->PlayAttack3Montage();
		}
		ServerAttack3();
	}
}

void UCombatComponent::Attack4Pressed(bool bPressed)
{
	bAttack4Pressed = bPressed;
	UE_LOG(LogTemp, Warning, TEXT("Attack 4"));
	if (bAttack4Pressed && bCanAttack)
	{
		
		bCanAttack = false;
		if (Character)
		{
			Character->bDisableMovement = true;
			Character->PlayAttack4Montage();
		}
		ServerAttack4();
	}
	
}

void UCombatComponent::ServerAttack1_Implementation()
{
	MulticastAttack1();
}

void UCombatComponent::ServerAttack2_Implementation()
{
	MulticastAttack2();
}

void UCombatComponent::ServerAttack3_Implementation()
{
	MulticastAttack3();
}

void UCombatComponent::ServerAttack4_Implementation()
{
	MulticastAttack4();
}

void UCombatComponent::MulticastAttack1_Implementation()
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	if (Character && !Character->IsLocallyControlled())
	{
		Character->bDisableMovement = true;
		bCanAttack = false;
		Character->PlayAttack1Montage();
	}
}

void UCombatComponent::MulticastAttack2_Implementation()
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	if (Character && !Character->IsLocallyControlled())
	{
		Character->bDisableMovement = true;
		bCanAttack = false;
		Character->PlayAttack2Montage();
	}
}

void UCombatComponent::MulticastAttack3_Implementation()
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	if (Character && !Character->IsLocallyControlled())
	{
		Character->bDisableMovement = true;
		bCanAttack = false;
		Character->PlayAttack3Montage();
	}
}

void UCombatComponent::MulticastAttack4_Implementation()
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	if (Character && !Character->IsLocallyControlled())
	{
		Character->bDisableMovement = true;
		bCanAttack = false;
		Character->PlayAttack4Montage();
	}
}




