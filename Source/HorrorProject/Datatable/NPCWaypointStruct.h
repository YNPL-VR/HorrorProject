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
	float WaitTime;
};