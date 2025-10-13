// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/ANS_AnimationRotate.h"

#include "Camera/CameraComponent.h"
#include "Character/MultiCharacter.h"

void UANS_AnimationRotate::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (AMultiCharacter* PlayerRef = Cast<AMultiCharacter>(MeshComp->GetOwner()))
	{
		FRotator Rotator;
		Rotator.Roll = PlayerRef->GetActorRotation().Roll;
		Rotator.Pitch = PlayerRef->GetActorRotation().Pitch;
		Rotator.Yaw = PlayerRef->GetFollowCamera()->GetComponentRotation().Yaw;
		
		PlayerRef->SetActorRotation(Rotator);
	}
}
