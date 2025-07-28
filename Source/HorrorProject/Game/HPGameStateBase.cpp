// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HPGameStateBase.h"

AHPGameStateBase::AHPGameStateBase()
{
	//DataTable √ ±‚»≠
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

	const FString ContextString(TEXT(" AHPGameStateBase::BeginPlay"));
	MinigameLevelDesignDataTable->GetAllRows(ContextString, MinigameLevelDesignData);

	for (const FMinigameLevelDesign* RowData : MinigameLevelDesignData)
	{
		if (RowData)
		{
			UE_LOG(LogTemp, Warning, TEXT("%f"), RowData->LevelUpSecondTimer);
		}
	}
}

void AHPGameStateBase::BeginDestroy()
{
	Super::BeginDestroy();

	for (FMinigameLevelDesign* MinigameLevelDesign : MinigameLevelDesignData)
	{
		MinigameLevelDesign = nullptr;
	}
}