#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DayBattery.generated.h"

USTRUCT(BlueprintType)
struct FDayBattery : public FTableRowBase
{
	GENERATED_BODY()

public:
	//�Ϸ簡 ���۵ɶ��� ���͸� �⺻��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayBattery")
	float DefaultBattery;
	//�˶��� �︮�� �Ҹ�Ǵ� ���͸��� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayBattery")
	float AlarmBattery;
	//�̴ϰ����� ���� ȹ���ϴ� ���͸��� �� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayBattery")
	float ChargeBattery;
};