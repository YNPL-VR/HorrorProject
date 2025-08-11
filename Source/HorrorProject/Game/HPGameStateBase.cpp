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
}

void AHPGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentMinigameLevel = 0;

	const FString ContextString(TEXT(" AHPGameStateBase::BeginPlay"));
	MinigameLevelDesignDataTable->GetAllRows(ContextString, MinigameLevelDesignData);

	for (const FMinigameLevelDesign* RowData : MinigameLevelDesignData)
	{
		if (RowData)
		{
			UE_LOG(LogTemp, Warning, TEXT("%f"), RowData->LevelUpSecondTimer);
		}
	}

	GetWorld()->GetTimerManager().SetTimer(NextLevelTimerHandle,this,&AHPGameStateBase::TimeToNextLevel, GetLevelUpSecondTimer(CurrentMinigameLevel));
}

void AHPGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	LevelUpAndNextTimeDynaicMultiDelegate.Clear();
	EndTimeDynaicMultiDelegate.Clear();
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
	if (LevelUpAndNextTimeDynaicMultiDelegate.IsBound())
	{
		LevelUpAndNextTimeDynaicMultiDelegate.Broadcast();
	}
	if (EndTimeDynaicMultiDelegate.IsBound() && CurrentMinigameLevel == MinigameLevelDesignData.Num() - 1)
	{
		EndTimeDynaicMultiDelegate.Broadcast();
	}
	
	if (CurrentMinigameLevel < MinigameLevelDesignData.Num())
	{
		GetWorld()->GetTimerManager().SetTimer(NextLevelTimerHandle, this, &AHPGameStateBase::TimeToNextLevel, GetLevelUpSecondTimer(CurrentMinigameLevel));
	}
}
