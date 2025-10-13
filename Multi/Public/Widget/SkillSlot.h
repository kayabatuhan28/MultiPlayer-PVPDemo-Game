// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillSlot.generated.h"


UCLASS()
class MULTI_API USkillSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UProgressBar* SkillProgressBar;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* SilenceImg;
	
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UTextBlock* SkillCooldownText;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UTextBlock* SkillKeyText;

	UFUNCTION(BlueprintImplementableEvent)
	void SetSkillCooldown(int InSkillCooldown);

	UFUNCTION(BlueprintImplementableEvent)
	void SetProgressBarSkillIcon(UTexture2D* InTexture);
	
};
