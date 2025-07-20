// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HGEnemyAIController.generated.h"

DECLARE_MULTICAST_DELEGATE(FAlreadyAtGoalDelegate);

/**
 * 
 */
UCLASS()
class HORRORPROJECT_API AHGEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AHGEnemyAIController();

	

	void ToDestination(FVector Point);

	FAlreadyAtGoalDelegate AlreadyAtGoalMultiDelegate;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	// 이동 완료시 호출되는 함수
	virtual void OnMoveCompleted(FAIRequestID RequestID,const FPathFollowingResult& Result) override;
private:
	FVector DestinationPoint;
	EPathFollowingRequestResult::Type PreResult;
	
};
