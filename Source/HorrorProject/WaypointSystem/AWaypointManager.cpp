// Fill out your copyright notice in the Description page of Project Settings.


#include "WaypointSystem/AWaypointManager.h"
#include "WaypointSystem/Waypoint.h"
#include "Enemy/HGCharacterEnemy.h"
#include "Enemy/HGEnemyAIController.h"

// Sets default values
AAWaypointManager::AAWaypointManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CurrentWaypoint = 0;

	//DataTable 초기화
	static ConstructorHelpers::FObjectFinder<UDataTable> NPCWaypointObject(TEXT("/Game/Datatable/NPCWaypoint"));
	if (NPCWaypointObject.Succeeded())
	{
		NPCWaypointDataTable = NPCWaypointObject.Object;
		

		UE_LOG(LogTemp, Warning, TEXT("NPCWaypointObjectDataTable Succeed!"));
	}
}

// Called when the game starts or when spawned
void AAWaypointManager::BeginPlay()
{
	Super::BeginPlay();
	
	const FString ContextString(TEXT("AAWaypointManager::BeginPlay"));
	TArray<FNPCWaypointStruct*> AllRows;
	//FNPCWaypointStruct* NPCWaypointStruct = NPCWaypointDataTable->FindRow<FNPCWaypointStruct>(FName("Time"), ContextString);

	NPCWaypointDataTable->GetAllRows(ContextString, AllRows);
	for (const FNPCWaypointStruct* RowData : AllRows)
	{
		if (RowData)
		{
			WaypointInfo.Add(RowData->WaitTime);
		}
	}

	//처음 WayPoint 이동
	if (Path.Num() > 0 && nullptr != TargetClass)
	{
		if (nullptr != Path[0])
		{
			FActorSpawnParameters spawnParams;
			Target = GetWorld()->SpawnActor<AHGCharacterEnemy>(TargetClass, Path[0]->GetActorLocation(), Path[0]->GetActorRotation(), spawnParams);
			CurrentWaypoint = 0;
			if (Path.Num() > 1)
			{
				AHGEnemyAIController* controller = Cast<AHGEnemyAIController>(Target->GetController());
				controller->AlreadyAtGoalMultiDelegate.AddDynamic(this, &AAWaypointManager::ArrivedOnHGWaypoint); //(this, FName("ArrivedOnHGWaypoint"));
				// FMath::FRandRange( 0 , 0 ) 일때 SetTimer가 작동하지 않는 버그 발생
				//const float RandomWaitTime = FMath::FRandRange(Path[CurrentWaypoint]->GetMinWaitSecond(), Path[CurrentWaypoint]->GetMaxWaitSecond());
				GetWorld()->GetTimerManager().SetTimer(PathHandle, this, &AAWaypointManager::ArrivedOnHGWaypoint, WaypointInfo[0], false);
			}
		}
	}
}



void AAWaypointManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(PathHandle);
}

void AAWaypointManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//#if WITH_EDITOR
//void AAWaypointManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//
//	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
//	if (PropertyName == GET_MEMBER_NAME_CHECKED(AAWaypointManager, Path))
//	{
//		UpdateWaypointInfo();
//	}
//
//}
//#endif

//Waypoint 에 도착했을 때 실행하는 함수
void AAWaypointManager::ArrivedOnHGWaypoint()
{
	GetWorld()->GetTimerManager().ClearTimer(PathHandle);
	
	//현재 Waypoint 갱신
	++CurrentWaypoint;

	
	
	//이동
	if (Target)
	{
		Target->SetActorLocation(Path[CurrentWaypoint]->GetActorLocation());
		Target->SetActorRotation(Path[CurrentWaypoint]->GetActorRotation());
		Target->SetState(EStateEnemy::Waiting);
	}

	//n 초 뒤 다음 Waypoint 로 이동 
	if (Path.Num() > CurrentWaypoint + 1 && WaypointInfo.Num() > CurrentWaypoint)
	{
		//const float RandomWaitTime = FMath::FRandRange(Path[CurrentWaypoint]->GetMinWaitSecond(), Path[CurrentWaypoint]->GetMaxWaitSecond());
		GetWorld()->GetTimerManager().SetTimer(PathHandle, this, &AAWaypointManager::ArrivedOnHGWaypoint, WaypointInfo[CurrentWaypoint], false);
	}
	else if(Path.Num() > CurrentWaypoint + 1 && WaypointInfo.Num() <= CurrentWaypoint)
	{
		UE_LOG(LogTemp, Error, TEXT("WaitTime 보다 Waypoint가 많아서 누락된 Waypoint가 있습니다."));
	}
}


//void AAWaypointManager::UpdateWaypointInfo()
//{
//	WaypointInfo.Empty();
//
//
//
//	{
//		/*
//		const FString ContextString(TEXT("ItemDataTable Context"));
//		FNPCWaypointStruct* NPCWaypointStruct = NPCWaypointDataTable->FindRow<FNPCWaypointStruct>(FName("Time"), ContextString);
//
//		if (Path.Num() > 0)
//		{
//			WaypointInfo.Reserve(Path.Num());
//			switch (Path.Num())
//			{
//			case 6:
//				WaypointInfo[5]=(NPCWaypointStruct->Waypoint5to6);
//			case 5:
//				WaypointInfo[4] = (NPCWaypointStruct->Waypoint4to5);
//			case 4:
//				WaypointInfo[3] = (NPCWaypointStruct->Waypoint3to4);
//			case 3:
//				WaypointInfo[2] = (NPCWaypointStruct->Waypoint2to3);
//			case 2:
//				WaypointInfo[1]= (NPCWaypointStruct->Waypoint1to2);
//			case 1:
//				WaypointInfo[0]= (NPCWaypointStruct->Waypoint0to1);
//			}
//		}*/
//
//		UE_LOG(LogTemp, Warning, TEXT("NPCWaypointObjectDataTable Succeed!"));
//	}
//
//	
//
//	/*for (const AWaypoint* Waypoint : Path)
//	{
//		if (Waypoint)
//		{
//			FWaypointInfo2 WaypointStruct;
//			WaypointStruct.MaxWaitSecond = Waypoint->GetMaxWaitSecond();
//			WaypointStruct.MinWaitSecond = Waypoint->GetMinWaitSecond();
//			WaypointStruct.Speed = Waypoint->GetSpeed();
//			WaypointInfo.Add(WaypointStruct);
//		}
//	}*/
//}

//void AAWaypointManager::MoveToNextWaypoint()
//{
//	UE_LOG(LogTemp, Warning, TEXT("MoveToNextWaypoint"));
//	if (Path.Num() > CurrentWaypoint + 1 && Target)
//	{
//		AHGEnemyAIController* controller = Cast<AHGEnemyAIController>(Target->GetController());
//		if (controller)
//		{
//			Target->SetState(EStateEnemy::Running);
//			Target->SetWalkSpeed(Path[CurrentWaypoint]->GetSpeed());
//			controller->ToDestination(Path[CurrentWaypoint + 1]->GetActorLocation());
//		}
//	}
//}