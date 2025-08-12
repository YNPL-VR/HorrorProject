// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BalloonWidget.h"
#include "Components/TextBlock.h"
void UBalloonWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void UBalloonWidget::SetTextNumber(int32 Number)
{
	UE_LOG(LogTemp, Warning, TEXT("%d"), Number);
	TxtNumber->SetText(FText::AsNumber(Number));
}
