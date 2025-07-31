// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interface/HPMinigameDataInterface.h"
#include "HPGameStateBase.generated.h"

//Todo : ��ư ���� -> �� ��������Ʈ �ڷ� �̵�, ���͸� ǥ�� UI -> ���� ����
/*
*	1. vr���� ��ư ������Ʈ ��ȣ�ۿ��Ͽ� 
*	2. �� ��������Ʈ �ڷ� �̵� �� ������ �Ҹ� -> WaypointManager���� �� ��������Ʈ �ڷ� �̵�, PlayerPawn���� ������ �Ҹ�
*	3. ���͸� UI ǥ��
*	4. ���� �¸��ÿ� ���͸� ���� �Լ�
*	5. GameState���� Battery�� Minigame���� GetSet �Լ� �����
* 
*	2025.07.31
*	playerPawn �ڵ�޽ÿ� sphere �ݸ��� ������Ʈ �߰��Ͽ� ��ư�� �����浹�ǰ� �����.
*	��ư�� �����ٴ� ��������Ʈ�� �ް� playerPawn ���� ���͸� �Ҹ� ���ɿ��θ� �˻��Ͽ� ����/�Ұ��� ��������Ʈ�� ���� Broadcast, WaypointManager���� ���� ��������Ʈ�� �ް� �ڷ��̵��ϴ� �Լ� �۵�
*/
/**
 * 
 */
UCLASS()
class HORRORPROJECT_API AHPGameStateBase : public AGameStateBase, public IHPMinigameDataInterface
{
	GENERATED_BODY()
public:
	AHPGameStateBase();

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	int32 GetMinigameLevel() override;
	float GetLevelUpSecondTimer(int32 InMinigameLevel) override;
	float GetConsumeAlarmBattery(int32 InMinigameLevel) override;
	float GetChargeBattery(int32 InMinigameLevel) override;
private:
	//UPROPERTY(VisibleAnywhere, Category = Path)
	TArray<FMinigameLevelDesign*> MinigameLevelDesignData;

	UPROPERTY()
	UDataTable* MinigameLevelDesignDataTable;

	int32 CurrentMinigameLevel;
};
