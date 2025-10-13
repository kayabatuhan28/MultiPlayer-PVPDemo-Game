// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Data/PlayerDataAsset.h"
#include "AN_SpawnSkill.generated.h"

class AMultiCharacter;

UCLASS()
class MULTI_API UAN_SpawnSkill : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SpawnedAct;

	UPROPERTY(EditAnywhere)
	FName SpawnSocketName;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ESkillType> SkillType;

	void SpawnProjectileSkill(AMultiCharacter* InPlayer);

	void SpawnAttachedSkill(AMultiCharacter* InPlayer);

	void SpawnNoRotateSkill(AMultiCharacter* InPlayer);

	void SpawnTargetAreaSkill(AMultiCharacter* InPlayer);
	
};
