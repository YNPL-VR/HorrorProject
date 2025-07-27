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
	float Waypoint0to1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float Waypoint1to2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float Waypoint2to3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float Waypoint3to4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float Waypoint4to5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float Waypoint5to6;

};