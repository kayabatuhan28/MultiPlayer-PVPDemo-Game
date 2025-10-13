// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AN_SpawnSkill.h"
#include "Character/MultiCharacter.h"

void UAN_SpawnSkill::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                            const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AMultiCharacter* Player = Cast<AMultiCharacter>(MeshComp->GetOwner()))
	{
		if (Player->HasAuthority())
		{
			if (SkillType == ESkillType::Projectile)
			{
				SpawnProjectileSkill(Player);
			}
			else if (SkillType == ESkillType::Attached)
			{
				SpawnAttachedSkill(Player);
			}
			else if (SkillType == ESkillType::NoRotate)
			{
				SpawnNoRotateSkill(Player);
			}
			else if (SkillType == ESkillType::TargetArea)
			{
				SpawnTargetAreaSkill(Player);
			}
		}
	}
}

void UAN_SpawnSkill::SpawnProjectileSkill(AMultiCharacter* InPlayer)
{
	FVector SpawnLocation = InPlayer->GetMesh()->GetSocketLocation(SpawnSocketName);
	FRotator SpawnRot = InPlayer->GetActorRotation();
	FTransform Transform(SpawnRot, SpawnLocation, FVector(1,1,1));
	
	InPlayer->Server_SpawnSkill(SpawnedAct, Transform, InPlayer, SkillType, SpawnSocketName);
}

void UAN_SpawnSkill::SpawnAttachedSkill(AMultiCharacter* InPlayer)
{
	FVector SpawnLocation = InPlayer->GetMesh()->GetSocketLocation(SpawnSocketName);
	FRotator SpawnRot = FRotator::ZeroRotator;
	FTransform Transform;
	Transform.SetLocation(SpawnLocation);
	Transform.SetRotation(SpawnRot.Quaternion());
	
	InPlayer->Server_SpawnSkill(SpawnedAct, Transform, InPlayer, SkillType, SpawnSocketName);
}

void UAN_SpawnSkill::SpawnNoRotateSkill(AMultiCharacter* InPlayer)
{
	FVector SpawnLocation = InPlayer->GetMesh()->GetSocketLocation(SpawnSocketName);
	FRotator SpawnRot = InPlayer->GetActorRotation();
	FTransform Transform;
	Transform.SetLocation(SpawnLocation);
	Transform.SetRotation(SpawnRot.Quaternion());
	
	InPlayer->Server_SpawnSkill(SpawnedAct, Transform, InPlayer, SkillType, SpawnSocketName);
}

void UAN_SpawnSkill::SpawnTargetAreaSkill(AMultiCharacter* InPlayer)
{
	FVector SpawnLocation = InPlayer->TraceLoc();
	FRotator SpawnRot = FRotator::ZeroRotator;
	FTransform Transform;
	Transform.SetLocation(SpawnLocation);
	Transform.SetRotation(SpawnRot.Quaternion());
	
	InPlayer->Server_SpawnSkill(SpawnedAct, Transform, InPlayer, SkillType, SpawnSocketName);
}
