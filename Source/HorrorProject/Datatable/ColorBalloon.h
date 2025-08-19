#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ColorBalloon.generated.h"

USTRUCT(BlueprintType)
struct FColorBalloon : public FTableRowBase
{
	GENERATED_BODY()

public:
	//1번부터 x번의 색이 등장할 때 x의 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorBalloon")
	int32 ColorNum;
	// 모니터에서 색이 노출될 때 노출되는 시간의 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorBalloon")
	float ColorChangeSpeed;
	//풍선의 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorBalloon")
	float BallonSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorBalloon")
	float LineRandomInterval;
};