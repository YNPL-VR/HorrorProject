// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Datatable/NPCWaypointStruct.h"
#include "AWaypointManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyReachEndPointMutiDelegate);

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
	UFUNCTION()
	void SuccessEndGame();

public:
	// Waypoint 설정할 때마다 에디터의 정보 갱신
//#if WITH_EDITOR
//	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif

protected:
	//InWaypoint 웨이포인트로 이동
	void MoveWaypointTarget(int32 InWaypoint);
	//웨이포인트에 도착했을때의 함수명
	UFUNCTION()
	void ArrivedOnHGWaypoint();

	UFUNCTION()
	void ResetWaypoint();
public:
	FEnemyReachEndPointMutiDelegate EnemyReachEndPointMutiDelegate;
private:
	UPROPERTY(EditAnywhere, Category = Path, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class AHGCharacterEnemy>> TargetClassArr;

	UPROPERTY()
	TArray<TObjectPtr<class AHGCharacterEnemy>> TargetArr;

	TObjectPtr<class AHGCharacterEnemy> CurrentTarget;

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
