// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interface/HPMinigameDataInterface.h"
#include "HPGameStateBase.generated.h"

//Todo : 버튼 액터 -> 적 웨이포인트 뒤로 이동, 배터리 표시 UI -> 감소 증가
/*
*	1. vr에서 버튼 오브젝트 상호작용하여 
*	2. 적 웨이포인트 뒤로 이동 후 에너지 소모 -> WaypointManager에서 적 웨이포인트 뒤로 이동, PlayerPawn에서 에너지 소모
*	3. 배터리 UI 표시
*	4. 게임 승리시에 배터리 충전 함수
*	5. GameState에서 Battery와 Minigame정보 GetSet 함수 만들기
* 
*	2025.07.31
*	playerPawn 핸드메시에 sphere 콜리전 컴포넌트 추가하여 버튼과 물리충돌되게 만든다.
*	버튼을 눌렀다는 델리게이트를 받고 playerPawn 에서 배터리 소모 가능여부를 검사하여 가능/불가능 델리게이트를 각각 Broadcast, WaypointManager에서 가능 델리게이트를 받고 뒤로이동하는 함수 작동
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
