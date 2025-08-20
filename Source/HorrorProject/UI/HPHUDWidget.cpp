// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HPHUDWidget.h"
#include "Player/HPPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Interface/HPMinigameDataInterface.h"
#include "Game/HPGameStateBase.h"
#include "EngineUtils.h"
#include "WaypointSystem/AWaypointManager.h"

void UHPHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//알람 성공 델리게이트 구독
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (AHPPawn* Player = Cast<AHPPawn>(PlayerPawn))
	{
		Player->UIConsumeBatteryDelegate.BindUFunction(this, FName("SetBatteryPercent"));
		Player->UIChargeBatteryDelegate.BindUFunction(this, FName("SetBatteryPercent"));
		SetBatteryPercent(Player->GetBattery());
	}

	TimeStrArray.Add("22:00");
	TimeStrArray.Add("00:00");
	TimeStrArray.Add("02:00");
	TimeStrArray.Add("04:00");
	TimeStrArray.Add("06:00");
	
	
	IHPMinigameDataInterface* gs = GetWorld()->GetGameState<IHPMinigameDataInterface>();
	if (gs)
	{
		// 레벨이 증가할 때마다 시간 흐름 업데이트
		gs->LevelUpAndNextTimeDynaicMultiDelegate.AddDynamic(this,&UHPHUDWidget::SetTime);
		// 클리어인지 확인 후 클리어메시지 표시
		gs->EndTimeDynaicMultiDelegate.AddDynamic(this, &UHPHUDWidget::SetVsisibleSuccessMsg);
		// 종료시간이 되었을 때 다음날로 변경
		gs->BeginNextDayMultiDelegate.AddDynamic(this, &UHPHUDWidget::SetDefaultTimeAndCurrentDay);
	}
	// 실패 메시지 표시
	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<AAWaypointManager> It(World); It; ++It)
		{
			AAWaypointManager* WaypointManager = *It;
			if (WaypointManager)
			{
				// 적이 플레이어를 공격할때 실패 메시지 표시
				WaypointManager->EnemyReachEndPointMutiDelegate.AddDynamic(this, &UHPHUDWidget::SetVsisibleFailMsg);
			}
			break;
		}
	}

}

void UHPHUDWidget::SetBatteryPercent(float InBattery)
{
	float CurrentBatteryPercent = InBattery / 100.0f;
	BatteryBar->SetPercent(CurrentBatteryPercent);
	FString BatteryStr = FString::FromInt(InBattery) + "%";
	TxtBatteryPercent->SetText(FText::FromString(BatteryStr));

}

void UHPHUDWidget::SetTime()
{
	IHPMinigameDataInterface* gs = GetWorld()->GetGameState<IHPMinigameDataInterface>();
	if (gs)
	{
		FString TimeStr = "Time : ";
		TimeStr += TimeStrArray[gs->GetMinigameLevel()];
		TxtTime->SetText(FText::FromString(TimeStr));
	}
}

void UHPHUDWidget::SetVsisibleSuccessMsg()
{
	IHPMinigameDataInterface* gs = GetWorld()->GetGameState<IHPMinigameDataInterface>();
	if (gs)
	{
		//게임 클리어인지 확인
		if (LASTDAY < gs->GetCurrentDay())
		{
			TxtSuccessMsg->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UHPHUDWidget::SetVsisibleFailMsg()
{
	TxtFailMsg->SetVisibility(ESlateVisibility::Visible);
}

void UHPHUDWidget::SetDefaultTimeAndCurrentDay()
{
	
	IHPMinigameDataInterface* gs = GetWorld()->GetGameState<IHPMinigameDataInterface>();
	if (gs)
	{
		//Day표시
		FString DayStr = "Day ";
		DayStr += FString::FromInt(gs->GetCurrentDay());
		TxtDay->SetText(FText::FromString(DayStr));

		//시간 초기화
		FString TimeStr = "Time : ";
		TimeStr += TimeStrArray[gs->GetMinigameLevel()];
		TxtTime->SetText(FText::FromString(TimeStr));
	
	}
}
