// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainOverlay.generated.h"

class USkillSlot;

UCLASS()
class MULTI_API UMainOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	USkillSlot* Skill1;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	USkillSlot* Skill2;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	USkillSlot* Skill3;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	USkillSlot* Skill4;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	USkillSlot* Skill5;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetSilenceImage(bool IsActive);
	
};
