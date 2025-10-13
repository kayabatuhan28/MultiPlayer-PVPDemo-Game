// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/ANS_SetMovementMode.h"

#include "Character/MultiCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UANS_SetMovementMode::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AMultiCharacter* PlayerRef = Cast<AMultiCharacter>(MeshComp->GetOwner()))
	{
		PlayerRef->GetCharacterMovement()->MovementMode = NewMovementMode;
	}
}

void UANS_SetMovementMode::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AMultiCharacter* PlayerRef = Cast<AMultiCharacter>(MeshComp->GetOwner()))
	{
		PlayerRef->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
	}
}
