#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NumBalloon.generated.h"

USTRUCT(BlueprintType)
struct FNumBalloon : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NumBalloon")
	int32 BalloonNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NumBalloon")
	float BalloonSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NumBalloon")
	float LineRandomInterval;
};