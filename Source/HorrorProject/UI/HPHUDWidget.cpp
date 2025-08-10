// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HPHUDWidget.h"
#include "Player/HPPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"


void UHPHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//알람 성공 델리게이트 등록
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (AHPPawn* Player = Cast<AHPPawn>(PlayerPawn))
	{
		Player->UIConsumeBatteryDelegate.BindUFunction(this, FName("SetBatteryPercent"));
	}

	TimeStrArray.Add("22:00");
	TimeStrArray.Add("00:00");
	TimeStrArray.Add("02:00");
	TimeStrArray.Add("04:00");
	TimeStrArray.Add("06:00");
}

void UHPHUDWidget::SetBatteryPercent(float InBattery)
{
	float CurrentBatteryPercent = InBattery / 100.0f;
	BatteryBar->SetPercent(CurrentBatteryPercent);
	FString BatteryStr = FString::FromInt(InBattery) + "%";
	TxtBatteryPercent->SetText(FText::FromString(BatteryStr));

}
