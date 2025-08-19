#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ColorBalloon.generated.h"

USTRUCT(BlueprintType)
struct FColorBalloon : public FTableRowBase
{
	GENERATED_BODY()

public:
	//1������ x���� ���� ������ �� x�� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorBalloon")
	int32 ColorNum;
	// ����Ϳ��� ���� ����� �� ����Ǵ� �ð��� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorBalloon")
	float ColorChangeSpeed;
	//ǳ���� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorBalloon")
	float BallonSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorBalloon")
	float LineRandomInterval;
};