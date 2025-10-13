// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/PlayerAnimInstance.h"

#include "Character/MultiCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerRef = Cast<AMultiCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (PlayerRef)
	{
		Speed = TryGetPawnOwner()->GetVelocity().Size();
		Direction = CalculateDirection(TryGetPawnOwner()->GetVelocity(),TryGetPawnOwner()->GetActorRotation());
		InAir = TryGetPawnOwner()->GetMovementComponent()->IsFalling();
	}
}
