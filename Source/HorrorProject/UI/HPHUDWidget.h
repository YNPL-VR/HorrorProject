// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class HORRORPROJECT_API UHPHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetBatteryPercent(float InBattery);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* BatteryBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TxtBatteryPercent;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TxtDay;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TxtTime;

	TArray<FString> TimeStrArray;
};
