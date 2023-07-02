#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied	UMETA(DisplayName = "Unoccupied"),
	ECS_Attacking	UMETA(DisplayName = "Attacking"),
	ECS_Blocking	UMETA(DisplayName = "Blocking"),
	ECS_Hitstun		UMETA(DisplayName = "Hitstun"),
	ECS_Blockstun	UMETA(DisplayName = "Blockstun"),
	ECS_Thrown		UMETA(DisplayName = "Thrown"),
	ECS_Dashing		UMETA(DisplayName = "Dashing"),
	ECS_KnockedDown	UMETA(DisplayName = "Knocked Down"),
	ECS_Recovery	UMETA(DisplayName = "Recovery"),
	ECS_GroundBounce UMETA(DisplayName = "Ground Bounce"),
	ECS_Launched	UMETA(DisplayName = "Launched"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};