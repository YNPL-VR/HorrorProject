// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HPGameStateBase.h"

AHPGameStateBase::AHPGameStateBase()
{
	//DataTable �ʱ�ȭ
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
	//���� Ŭ������ ����
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

// ���� ���� ���̵��� ����
void AHPGameStateBase::TimeToNextLevel()
{

	GetWorld()->GetTimerManager().ClearTimer(NextLevelTimerHandle);
	++CurrentMinigameLevel;
	//���̵� �� �ð� ���� �˸�
	if (LevelUpAndNextTimeDynaicMultiDelegate.IsBound())
	{
		LevelUpAndNextTimeDynaicMultiDelegate.Broadcast();
	}


	//���� �ð� ����
	if (CurrentMinigameLevel < MinigameLevelDesignData.Num())
	{
		GetWorld()->GetTimerManager().SetTimer(NextLevelTimerHandle, this, &AHPGameStateBase::TimeToNextLevel, GetLevelUpSecondTimer(CurrentMinigameLevel));
	}
	//����ð��� �Ǿ��ٰ� �˸�
	else if (CurrentMinigameLevel == MinigameLevelDesignData.Num())
	{
		//������ ���� �ƴ϶�� ���� ������ �ʱ�ȭ
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

		//n�� �ڿ� ǥ��
		FTimerHandle EndDayTimer;
		GetWorld()->GetTimerManager().SetTimer(EndDayTimer, this, &AHPGameStateBase::SetNextDay, 3.0f);
	}
	
}
