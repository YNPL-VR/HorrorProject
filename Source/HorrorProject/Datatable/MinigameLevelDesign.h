#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MinigameLevelDesign.generated.h"

USTRUCT(BlueprintType)
struct FMinigameLevelDesign : public FTableRowBase
{
	GENERATED_BODY()

public:
	//미니게임 난이도 업 타이머
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float LevelUpSecondTimer;

	//알람을 울리면 소모되는 배터리의 양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float AlarmBattery;
	//미니게임을 통해 획득하는 배터리의 양 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float ChargeBattery;
};