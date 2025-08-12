// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BalloonWidget.generated.h"

/**
 * 
 */
UCLASS()
class HORRORPROJECT_API UBalloonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void SetTextNumber(int32 Number);
private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* TxtNumber;
};
