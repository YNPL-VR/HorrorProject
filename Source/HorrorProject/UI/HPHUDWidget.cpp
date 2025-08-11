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

	//�˶� ���� ��������Ʈ ����
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
	
	
	IHPMinigameDataInterface* gs = GetWorld()->GetGameState<IHPMinigameDataInterface>();
	if (gs)
	{
		// ������ ������ ������ �ð� �帧 ������Ʈ
		gs->LevelUpAndNextTimeDynaicMultiDelegate.AddDynamic(this,&UHPHUDWidget::SetTime);
		// �ð��� �� �Ǿ��� �� ���� �޼��� ǥ��
		gs->EndTimeDynaicMultiDelegate.AddDynamic(this, &UHPHUDWidget::SetVsisibleSuccessMsg);
	}
	// ���� �޽��� ǥ��
	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<AAWaypointManager> It(World); It; ++It)
		{
			AAWaypointManager* WaypointManager = *It;
			if (WaypointManager)
			{
				// ���� �÷��̾ �����Ҷ� ���� �޽��� ǥ��
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
	TxtSuccessMsg->SetVisibility(ESlateVisibility::Visible);
}

void UHPHUDWidget::SetVsisibleFailMsg()
{
	TxtFailMsg->SetVisibility(ESlateVisibility::Visible);
}
