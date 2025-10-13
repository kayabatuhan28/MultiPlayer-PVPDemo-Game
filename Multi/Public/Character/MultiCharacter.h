// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/PlayerDataAsset.h"
#include "GameFramework/Character.h"
#include "Widget/DamageWidget.h"
#include "MultiCharacter.generated.h"

class UWidgetComponent;
class UOnHeadWidget;

UCLASS(config=Game)
class AMultiCharacter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
public:
	AMultiCharacter();

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EWeaponType> PlayerWeaponType = EWeaponType::NoWeapon;

	// Set Data in DataAsset
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data Asset")
	UPlayerDataAsset* PlayerDataAsset;

	UPROPERTY(BlueprintReadWrite)
	bool bIsDataSetted = false;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetCharacterData(UPlayerDataAsset* DataAsset);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_SetCharacterData(UPlayerDataAsset* DataAsset);

	// On Head Widget
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* OnHeadWidgetComp;
	UPROPERTY(BlueprintReadWrite)
	UOnHeadWidget* OnHeadWidget;

	// Damage Text Widget
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* DamageWidgetComp;
	UPROPERTY(BlueprintReadWrite)
	UDamageWidget* DamageWidget;

	// Main Character Overlay Widget
	UPROPERTY(EditAnywhere,Category="Widget")
	TSubclassOf<UUserWidget> MainOverlayClass;
	UPROPERTY(BlueprintReadWrite,Category="Widget")
	class UMainOverlay* MainOverlayWidget;

	// CrossHair Widget
	UPROPERTY(EditAnywhere,Category="Widget")
	TSubclassOf<UUserWidget> CrossHairWidgetClass;
	UPROPERTY(BlueprintReadWrite,Category="Widget")
	class UCrosshair* CrossHairWidget;

	

	UFUNCTION(BlueprintCallable)
	void FillImage(UPlayerDataAsset* InDataAsset);

	// Player Active State (Attack, dodge etc.)
	UPROPERTY(BlueprintReadWrite, Category = "ActiveState")
	TEnumAsByte<EActiveState> ActiveState;

	UPROPERTY(BlueprintReadWrite, Category = "Actions")
	FActiveActions Actions;

	// Check Pressed Direction Key and calculate direction
	UPROPERTY(BlueprintReadOnly)
	float ForwardValue;

	UPROPERTY(BlueprintReadOnly)
	float RightValue;

	float CalculateDirection();

	
	
	// Player Data
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	float CurrentStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	float MaxStamina;

	int LightAttackCount = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TArray<UAnimMontage*> M_BasicAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	UAnimMontage* M_RunAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	float RunAttackCd;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TArray<UAnimMontage*> M_SkillAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TArray<UTexture2D*> M_SkillImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TArray<float> M_SkillCd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TArray<bool> IsSkillCd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TArray<UAnimMontage*> M_Dodge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	float DodgeCd;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TArray<UAnimMontage*> M_HitMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TArray<UAnimMontage*> M_BlockHit;
	
	// Play Replicated Montage
	UFUNCTION(Server, Reliable)
	void Server_PlayMontage(UAnimMontage* Montage);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(UAnimMontage* Montage);

	// Spawn Replicated Skill Actor
	UFUNCTION(Server, Reliable)
	void Server_SpawnSkill(TSubclassOf<AActor> Class, FTransform SpawnTransform, AActor* SkillOwner, ESkillType InSkillType, FName InAttachSocket);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnSkill(TSubclassOf<AActor> Class, FTransform SpawnTransform, AActor* SkillOwner, ESkillType InSkillType,  FName InAttachSocket);

	// Trace
	FVector TraceLoc();

	// Set Replicated Blocking Variable
	UFUNCTION(Server, Reliable)
	void Server_SetBlock(bool IsActive);

	UFUNCTION(NetMulticast,Reliable)
	void Multicast_SetBlock(bool IsActive);

	// Clear Dodge Cd
	void ClearDodgeCd();

	// Reset Skill Cooldown
	void ResetSkill1Cd();
	void ResetSkill2Cd();
	void ResetSkill3Cd();
	void ResetSkill4Cd();
	void ResetSkill5Cd();

	// Clear Run Attack Cd
	void ClearRunAttackCd();

	// TakeHit
	UFUNCTION(BlueprintCallable)
	void TakeHit(float TakingDamage, int HitMontageIndex, int BlockMontageIndex);

	// Update Skill Cooldown Progressbar and text widget
	void SetSkillCooldownWidget(int InSkillIndex);

	// Extra effects that can be added to skills
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ApplyEffect(EEffectType InType, float LifeTime);

	// Archer Target Area and Rain Arrow Test
	UFUNCTION(BlueprintImplementableEvent)
	void RainArrow();

	void TestRainArrow();

	// Target Area Actor
	UPROPERTY(EditDefaultsOnly, Category = "TargetArea")
	TSubclassOf<AActor> TargetAreaClass;

	UPROPERTY(BlueprintReadWrite)
	AActor* TargetArea;

	void SetTargetAreaLoc();

	FTimerHandle SetTargetAreaLocHandle;

	// Set Replicated Steam Name
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetSteamName();

	// Stamina Regerenation
	FTimerHandle StaminaRegenHandle;

	void StaminaRegen();

	// Dead
	bool bIsDead;

	UFUNCTION(BlueprintImplementableEvent)
	void OnDead();

	

	

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void LeftMousePressed();
	
	void SprintPressed();
	void SprintReleased();
	
	void Dodge();

	DECLARE_DELEGATE_OneParam(FKeyPressedDelegate, int);
	void CastSkill(int InSkillIndex);

	DECLARE_DELEGATE_OneParam(FKeyReleasedDelegate, int);
	void CastSkillReleased(int InSkillIndex);
	
	void BlockPressed();
	void BlockReleased();

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

