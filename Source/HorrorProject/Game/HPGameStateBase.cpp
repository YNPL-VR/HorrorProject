// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HPGameStateBase.h"

AHPGameStateBase::AHPGameStateBase()
{
	//DataTable 초기화
	static ConstructorHelpers::FObjectFinder<UDataTable> MinigameLevelDesignDataTableFinder(TEXT("/Game/Datatable/MinigameLevelDesign.MinigameLevelDesign"));
	if (MinigameLevelDesignDataTableFinder.Succeeded())
	{
		MinigameLevelDesignDataTable = MinigameLevelDesignDataTableFinder.Object;
		UE_LOG(LogTemp, Warning, TEXT("MinigaemLevelDesignDataTable Succeed!"));
	}

	CurrentMinigameLevel = 0;
	CurrentDay = 1;
}

void AHPGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	
	const FString ContextString(TEXT(" AHPGameStateBase::BeginPlay"));
	MinigameLevelDesignDataTable->GetAllRows(ContextString, MinigameLevelDesignData);

	/*for (const FMinigameLevelDesign* RowData : MinigameLevelDesignData)
	{
		if (RowData)
		{
			UE_LOG(LogTemp, Warning, TEXT("%f"), RowData->LevelUpSecondTimer);
		}
	}*/

	GetWorld()->GetTimerManager().SetTimer(NextLevelTimerHandle,this,&AHPGameStateBase::TimeToNextLevel, GetLevelUpSecondTimer(CurrentMinigameLevel));
}

void AHPGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LevelUpAndNextTimeDynaicMultiDelegate.Clear();
	EndTimeDynaicMultiDelegate.Clear();
	BeginNextDayMultiDelegate.Clear();
	GetWorld()->GetTimerManager().ClearTimer(NextLevelTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void AHPGameStateBase::BeginDestroy()
{
	Super::BeginDestroy();

	for (FMinigameLevelDesign* MinigameLevelDesign : MinigameLevelDesignData)
	{
		MinigameLevelDesign = nullptr;
	}
}

int32 AHPGameStateBase::GetMinigameLevel()
{
	return CurrentMinigameLevel;
}

int32 AHPGameStateBase::GetCurrentDay()
{
	return CurrentDay;
}

void AHPGameStateBase::ToNextDay()
{
	++CurrentDay;
}

void AHPGameStateBase::SetNextDay()
{
	//게임 클리어라면 리턴
	if (CurrentDay > LASTDAY)
		return;
	if (BeginNextDayMultiDelegate.IsBound())
	{
		BeginNextDayMultiDelegate.Broadcast();
	}
}

float AHPGameStateBase::GetLevelUpSecondTimer(int32 InMinigameLevel)
{
	return MinigameLevelDesignData[InMinigameLevel]->LevelUpSecondTimer;
}

float AHPGameStateBase::GetConsumeAlarmBattery(int32 InMinigameLevel)
{
	return MinigameLevelDesignData[InMinigameLevel]->AlarmBattery;
}

float AHPGameStateBase::GetChargeBattery(int32 InMinigameLevel)
{
	return MinigameLevelDesignData[InMinigameLevel]->ChargeBattery;
}

// 다음 레벨 난이도로 설정
void AHPGameStateBase::TimeToNextLevel()
{

	GetWorld()->GetTimerManager().ClearTimer(NextLevelTimerHandle);
	++CurrentMinigameLevel;
	//난이도 및 시간 변경 알림
	if (LevelUpAndNextTimeDynaicMultiDelegate.IsBound())
	{
		LevelUpAndNextTimeDynaicMultiDelegate.Broadcast();
	}


	//다음 시간 설정
	if (CurrentMinigameLevel < MinigameLevelDesignData.Num())
	{
		GetWorld()->GetTimerManager().SetTimer(NextLevelTimerHandle, this, &AHPGameStateBase::TimeToNextLevel, GetLevelUpSecondTimer(CurrentMinigameLevel));
	}
	//종료시간이 되었다고 알림
	else if (CurrentMinigameLevel == MinigameLevelDesignData.Num())
	{
		//마지막 날이 아니라면 기존 데이터 초기화
		if (CurrentDay <= LASTDAY)
		{
			ToNextDay();
			CurrentMinigameLevel = 0;
			GetWorld()->GetTimerManager().SetTimer(NextLevelTimerHandle, this, &AHPGameStateBase::TimeToNextLevel, GetLevelUpSecondTimer(CurrentMinigameLevel) + 3.0f);
		}


		if (EndTimeDynaicMultiDelegate.IsBound())
		{
			EndTimeDynaicMultiDelegate.Broadcast();
		}

		//n초 뒤에 표시
		FTimerHandle EndDayTimer;
		GetWorld()->GetTimerManager().SetTimer(EndDayTimer, this, &AHPGameStateBase::SetNextDay, 3.0f);
	}
	
}
