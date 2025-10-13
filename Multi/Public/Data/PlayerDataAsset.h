// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerDataAsset.generated.h"

UENUM(BlueprintType)
enum EWeaponType
{
	NoWeapon					UMETA(DisplayName = "Empty"),
	GreatSword					UMETA(DisplayName = "GreatSword"),
	Archer						UMETA(DisplayName = "Archer"),
};

UENUM(BlueprintType)
enum EEffectType
{
	NoEffect					UMETA(DisplayName = "NoEffect"),
	Burning						UMETA(DisplayName = "Burning"),
	Slow						UMETA(DisplayName = "Slow"),
	Silence						UMETA(DisplayName = "Silence"),
};

UENUM(BlueprintType)
enum ESkillType
{
	NoSkillType			UMETA(DisplayName = "NoSkillType"),
	NoRotate			UMETA(DisplayName = "NoRotate"),
	Projectile			UMETA(DisplayName = "Projectile"),
	Attached			UMETA(DisplayName = "Attached"),
	TargetArea			UMETA(DisplayName = "TargetArea"),
	
};

USTRUCT(BlueprintType)
struct FActiveActions
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsSprinting;

	UPROPERTY(BlueprintReadWrite)
	bool bIsBlocking;

	UPROPERTY(BlueprintReadWrite)
	bool bCanDodge;

	UPROPERTY(BlueprintReadWrite)
	bool bCanRunAttack;

	UPROPERTY(BlueprintReadWrite)
	bool bCanHitReaction;

	UPROPERTY(BlueprintReadWrite)
	bool bIsSilence;
	
	FActiveActions()
	{
		bIsSprinting = false;
		bIsBlocking = false;
		bCanDodge = true;
		bCanRunAttack = true;
		bCanHitReaction = true;
		bIsSilence = false;
	};
	
};

UENUM(BlueprintType)
enum EActiveState
{
	Empty						UMETA(DisplayName = "Empty"),
	Attack						UMETA(DisplayName = "Attack"),
	Hitted						UMETA(DisplayName = "Hitted"),
	Dodge						UMETA(DisplayName = "Dodge"),
	
};


UCLASS(BlueprintType)
class UPlayerDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Class Type")
	TEnumAsByte<EWeaponType> WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Class Type")
	UStaticMesh* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Class Type")
	FName WeaponSocket;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Main Attributes")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Main Attributes")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Main Attributes")
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Main Attributes")
	float CurrentStamina;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TArray<UAnimMontage*> M_BasicAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	UAnimMontage* M_SprintAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	float SprintAttackCd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	TArray<UAnimMontage*> M_SkillAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	TArray<UTexture2D*> M_SkillImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	TArray<float> M_SkillCd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dodge")
	TArray<UAnimMontage*> M_Dodge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dodge")
	float DodgeCd;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
	TArray<UAnimMontage*> M_HitMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
	TArray<UAnimMontage*> M_BlockHit;
 
};