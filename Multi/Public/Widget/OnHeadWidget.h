// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/PlayerDataAsset.h"
#include "OnHeadWidget.generated.h"



UCLASS()
class MULTI_API UOnHeadWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealth(float InCurrentHealth, float InMaxHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateStamina(float InCurrentStamina, float InMaxStamina);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateSteamName(const FText& SelectedUsername);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetEffectIcon(EEffectType InEffectType, bool bIsVisible);
	
};
