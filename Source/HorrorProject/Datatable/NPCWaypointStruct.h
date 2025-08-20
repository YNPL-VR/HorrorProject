#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NPCWaypointStruct.generated.h"

USTRUCT(BlueprintType)
struct FNPCWaypointStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float Day1WaitTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float Day2WaitTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float Day3WaitTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float Day4WaitTime;
};