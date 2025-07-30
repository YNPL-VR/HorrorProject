// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Datatable/NPCWaypointStruct.h"
#include "AWaypointManager.generated.h"

//USTRUCT(Atomic, BlueprintType)
//struct FWaypointInfo2
//{
//	GENERATED_USTRUCT_BODY()
//
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float MinWaitSecond;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float MaxWaitSecond;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float Speed;
//};

UCLASS()
class HORRORPROJECT_API AAWaypointManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAWaypointManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetCurrentWaypoint(int32 InWaypoint) { CurrentWaypoint = InWaypoint; }
	FORCEINLINE int GetCurrentWaypoint() const { return CurrentWaypoint; }
	UFUNCTION()
	void MovePreviousWaypointTarget();

public:
	// 에디터에서 프로퍼티가 변경되었을 때 호출될 함수입니다.
//#if WITH_EDITOR
//	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif

protected:
	//InWaypoint 웨이포인트로 이동
	void MoveWaypointTarget(int32 InWaypoint);
	//웨이포인트에 도착했을때의 함수명
	UFUNCTION()
	void ArrivedOnHGWaypoint();

	
	// WaypointInfo 을 업데이트하는 함수.
	//void UpdateWaypointInfo();
	//다음 웨이포인트로 이동하는 함수
	//void MoveToNextWaypoint();

private:
	UPROPERTY(EditAnywhere, Category = Path, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AHGCharacterEnemy> TargetClass;

	UPROPERTY()
	TObjectPtr<class AHGCharacterEnemy> Target;

	UPROPERTY(EditAnywhere, Category = Path, meta = (AllowPrivateAccess = "true", DisplayName = "BP_Waypoint"))
	TArray<TObjectPtr<class AWaypoint>> Path;

	UPROPERTY(VisibleAnywhere, Category = Path)
	TArray<float> WaypointInfo;

	UPROPERTY(VisibleInstanceOnly, Category = Path)
	int32 CurrentWaypoint;

	FTimerHandle PathHandle;

	UPROPERTY()
	UDataTable* NPCWaypointDataTable;

};
