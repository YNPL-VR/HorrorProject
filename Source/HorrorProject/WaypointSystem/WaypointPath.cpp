// Fill out your copyright notice in the Description page of Project Settings.


#include "WaypointPath.h"
#include "Waypoint.h"
#include "Enemy/HGCharacterEnemy.h"
#include "Enemy/HGEnemyAIController.h"
// Sets default values
AWaypointPath::AWaypointPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AWaypointPath::BeginPlay()
{
	Super::BeginPlay();

	//처음 WayPoint 이동
	if (Path.Num() > 0 && nullptr != TargetClass)
	{
		if (nullptr!=Path[0])
		{
			FActorSpawnParameters spawnParams;
			Target = GetWorld()->SpawnActor<AHGCharacterEnemy>(TargetClass, Path[0]->GetActorLocation(), Path[0]->GetActorRotation(), spawnParams);
			currentWaypoint = 0;
			if (Path.Num() > 1)
			{
				AHGEnemyAIController* controller = Cast<AHGEnemyAIController>(Target->GetController());
				controller->AlreadyAtGoalMultiDelegate.AddUFunction(this, FName("ArrivedOnWaypoint"));
				// FMath::FRandRange( 0 , 0 ) 일때 SetTimer가 작동하지 않는 버그 발생
				const float RandomWaitTime = FMath::FRandRange(Path[currentWaypoint]->GetMinWaitSecond(), Path[currentWaypoint]->GetMaxWaitSecond());
				GetWorld()->GetTimerManager().SetTimer(PathHandle, this, &AWaypointPath::MoveToNextWaypoint, RandomWaitTime, false);
			}
			
		}
	}
	
	
}

void AWaypointPath::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(PathHandle);
}

// Called every frame
void AWaypointPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
#if WITH_EDITOR
void AWaypointPath::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AWaypointPath, Path))
	{
		UpdateWaypointInfo();
	}

}

//Waypoint 에 도착했을 때 실행하는 함수
void AWaypointPath::ArrivedOnWaypoint()
{
	GetWorld()->GetTimerManager().ClearTimer(PathHandle);

	//현재 Waypoint 갱신
	++currentWaypoint;

	if (Target)
	{
		Target->SetState(EStateEnemy::Waiting);
	}
	

	//n초 뒤 다음 Waypoint로 이동 
	if (WaypointInfo.Num() > currentWaypoint + 1)
	{
		const float RandomWaitTime = FMath::FRandRange(Path[currentWaypoint]->GetMinWaitSecond(), Path[currentWaypoint]->GetMaxWaitSecond());
		GetWorld()->GetTimerManager().SetTimer(PathHandle, this, &AWaypointPath::MoveToNextWaypoint, RandomWaitTime , false);
	}
	

}

#endif
void AWaypointPath::UpdateWaypointInfo()
{
	WaypointInfo.Empty();

	for (const AWaypoint* Waypoint : Path)
	{
		if (Waypoint)
		{
			FWaypointInfo WaypointStruct;
			WaypointStruct.MaxWaitSecond = Waypoint->GetMaxWaitSecond();
			WaypointStruct.MinWaitSecond = Waypoint->GetMinWaitSecond();
			WaypointStruct.Speed = Waypoint->GetSpeed();
			WaypointInfo.Add(WaypointStruct);
		}
	}
}

void AWaypointPath::MoveToNextWaypoint()
{
	UE_LOG(LogTemp, Warning, TEXT("MoveToNextWaypoint"));
	if (Path.Num() > currentWaypoint + 1 && Target)
	{
		AHGEnemyAIController* controller = Cast<AHGEnemyAIController>(Target->GetController());
		if (controller)
		{
			Target->SetState(EStateEnemy::Running);
			Target->SetWalkSpeed(Path[currentWaypoint]->GetSpeed());
			controller->ToDestination(Path[currentWaypoint + 1]->GetActorLocation());
		}
	}
}

