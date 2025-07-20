// Fill out your copyright notice in the Description page of Project Settings.


#include "HGEnemyAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "HGCharacterEnemy.h"

AHGEnemyAIController::AHGEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AHGEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	if (nullptr != GetOwner())
	{
		DestinationPoint = GetOwner()->GetActorLocation();
	}
	
}

void AHGEnemyAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AlreadyAtGoalMultiDelegate.Clear();
}

void AHGEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	//switch (Result)
	//{
	//case EPathFollowingRequestResult::AlreadyAtGoal:
	//	if (PreResult != EPathFollowingRequestResult::AlreadyAtGoal && nullptr != GetOwner())
	//	{
	//		AHGCharacterEnemy* Body = Cast<AHGCharacterEnemy>(GetOwner());
	//		// 도착 알림 다음 루트로 이동하는 코드
	//		AlreadyAtGoalDelegate.Broadcast();
	//		Body->SetState(EStateEnemy::Waiting);
	//		PreResult = EPathFollowingRequestResult::AlreadyAtGoal;
	//	}
	//	break;
	//case EPathFollowingRequestResult::Failed:
	//	UE_LOG(LogTemp, Warning, TEXT("AHGEnemyAIController EPathFollowingRequestResult::Failed"));
	//	break;
	//case EPathFollowingRequestResult::RequestSuccessful:
	//	PreResult = EPathFollowingRequestResult::RequestSuccessful;
	//	break;
	//default:
	//	break;
	//}
}

void AHGEnemyAIController::ToDestination(FVector Point)
{
	DestinationPoint = Point;
	MoveToLocation(DestinationPoint);
}

void AHGEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.Code == EPathFollowingResult::Success)
	{
		PreResult = EPathFollowingRequestResult::AlreadyAtGoal;

		// 도착 알림 다음 루트로 이동하는 코드
		if (AlreadyAtGoalMultiDelegate.IsBound() == true)
		{
			AlreadyAtGoalMultiDelegate.Broadcast();
		}
	}

}
