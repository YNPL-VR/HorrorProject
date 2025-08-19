#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DayBattery.generated.h"

USTRUCT(BlueprintType)
struct FDayBattery : public FTableRowBase
{
	GENERATED_BODY()

public:
	//하루가 시작될때의 배터리 기본값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayBattery")
	float DefaultBattery;
	//알람을 울리면 소모되는 배터리의 양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayBattery")
	float AlarmBattery;
	//미니게임을 통해 획득하는 배터리의 양 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayBattery")
	float ChargeBattery;
};