// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/ANS_SetActiveState.h"

#include "Character/MultiCharacter.h"

void UANS_SetActiveState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AMultiCharacter* PlayerRef = Cast<AMultiCharacter>(MeshComp->GetOwner()))
	{
		PlayerRef->ActiveState = NewState;
		PlayerRef->Actions.bCanHitReaction = CanHitReact;
	}
}

void UANS_SetActiveState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AMultiCharacter* PlayerRef = Cast<AMultiCharacter>(MeshComp->GetOwner()))
	{
		PlayerRef->ActiveState = EActiveState::Empty;
		PlayerRef->Actions.bCanHitReaction = true;
	}
}
