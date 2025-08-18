// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Datatable/MinigameLevelDesign.h"
#include "HPMinigameDataInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHPMinigameDataInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HORRORPROJECT_API IHPMinigameDataInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual int32 GetMinigameLevel() = 0;

	UFUNCTION()
	virtual float GetLevelUpSecondTimer(int32 InMinigameLevel) = 0;

	UFUNCTION()
	virtual float GetConsumeAlarmBattery(int32 InMinigameLevel) = 0;

	UFUNCTION()
	virtual float GetChargeBattery(int32 InMinigameLevel) = 0;
};