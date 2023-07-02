#pragma once

#include "Engine/DataTable.h"
#include "HitboxData.generated.h"

UENUM(BlueprintType)
enum class EHitboxType : uint8
{
	EHT_Proximity	UMETA(DisplayName = "Proximity"),
	EHT_Strike		UMETA(DisplayName = "Strike"),
	EHT_Hurtbox		UMETA(DisplayName = "Hurtbox"),
	EHT_Projectile	UMETA(DisplayName = "Projectile"),
	EHT_Throw		UMETA(DisplayName = "Throw"),

	EHT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EHitboxHeight : uint8
{
	EHH_Low		UMETA(DisplayName = "Low"),
	EHH_Mid		UMETA(DisplayName = "Mid"),
	EHH_High	UMETA(DisplayName = "High"),

	EHH_MAX		UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FHitboxData : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
		EHitboxType HitboxType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
		EHitboxHeight HitboxHeight = EHitboxHeight::EHH_Mid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
		FVector Location = FVector(60.f, 0., 0.f);;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
		FVector Scale = FVector(1.f, 0.05f, 0.6f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
		float HitstunTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
		float BlockstunTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
		float PushbackDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
		float LaunchDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
		bool bCauseGroundBounce;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
		bool bShouldAttachToCharacter = false;
};