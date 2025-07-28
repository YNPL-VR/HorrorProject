// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Datatable/MinigameLevelDesign.h"
#include "HPGameStateBase.generated.h"

//Todo : ��ư ���� -> �� ��������Ʈ �ڷ� �̵�, ���͸� ǥ�� UI -> ���� ����
/**
 * 
 */
UCLASS()
class HORRORPROJECT_API AHPGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	AHPGameStateBase();

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
private:
	//UPROPERTY(VisibleAnywhere, Category = Path)
	TArray<FMinigameLevelDesign*> MinigameLevelDesignData;

	UPROPERTY()
	UDataTable* MinigameLevelDesignDataTable;
};
