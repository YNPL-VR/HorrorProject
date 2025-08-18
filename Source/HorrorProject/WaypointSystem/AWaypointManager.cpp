// Fill out your copyright notice in the Description page of Project Settings.


#include "WaypointSystem/AWaypointManager.h"
#include "WaypointSystem/Waypoint.h"
#include "Enemy/HGCharacterEnemy.h"
#include "Enemy/HGEnemyAIController.h"
#include "Player/HPPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Game/HPGameStateBase.h"
#include "Interface/HPMinigameDataInterface.h"

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
	
	//각 Waypoint 데이터 저장
	const FString ContextString(TEXT("AAWaypointManager::BeginPlay"));
	TArray<FNPCWaypointStruct*> AllRows;
	NPCWaypointDataTable->GetAllRows(ContextString, AllRows);
	for (const FNPCWaypointStruct* RowData : AllRows)
	{
		if (RowData)
		{
			WaypointInfo.Add(RowData->WaitTime);
		}
	}

	//몬스터 생성 및 리셋하여 웨이포인트 시작
	if (Path.Num() > 0 && 0 < TargetClassArr.Num())
	{
		if (nullptr != Path[0])
		{
			//
			for (int32 SpawnCount = 0; SpawnCount < TargetClassArr.Num(); ++SpawnCount)
			{
				FActorSpawnParameters spawnParams;
				TargetArr.Add(GetWorld()->SpawnActor<AHGCharacterEnemy>(TargetClassArr[SpawnCount], Path[0]->GetActorLocation(), Path[0]->GetActorRotation(), spawnParams));
				TargetArr[SpawnCount]->SetActorHiddenInGame(true);
				/* 미사용 : 걸어다닐때의 로직  - 순간이동 로직을 사용해서
				if (Path.Num() > 1)
				{
					//AHGEnemyAIController* controller = Cast<AHGEnemyAIController>(TargetArr[SpawnCount]->GetController());
					//controller->AlreadyAtGoalMultiDelegate.AddDynamic(this, &AAWaypointManager::ArrivedOnHGWaypoint); //(this, FName("ArrivedOnHGWaypoint"));
					// FMath::FRandRange( 0 , 0 ) 일때 SetTimer가 작동하지 않음
					//const float RandomWaitTime = FMath::FRandRange(Path[CurrentWaypoint]->GetMinWaitSecond(), Path[CurrentWaypoint]->GetMaxWaitSecond());
				}
				*/
			}
			CurrentTarget = TargetArr[0];
			ResetWaypoint();
		}
	}

	//알람 성공 델리게이트 등록
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (AHPPawn* Player = Cast<AHPPawn>(PlayerPawn))
	{
		Player->SuccessConsumeBatteryDelegate.AddUFunction(this, FName("MovePreviousWaypointTarget"));
	}
	
	if (IHPMinigameDataInterface* gs = Cast<IHPMinigameDataInterface>(GetWorld()->GetGameState()))
	{
		//다음날 변경시 웨이포인트 리셋 및 적 변경
		gs->BeginNextDayMultiDelegate.AddDynamic(this,&AAWaypointManager::ResetWaypoint);
		//시간 마감시 적 이동하지 않게 만들기
		gs->EndTimeDynaicMultiDelegate.AddDynamic(this, &AAWaypointManager::SuccessEndGame);
	}
}



void AAWaypointManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(PathHandle);
	EnemyReachEndPointMutiDelegate.Clear();
	Super::EndPlay(EndPlayReason);
}

void AAWaypointManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAWaypointManager::MovePreviousWaypointTarget()
{
	if (nullptr == CurrentTarget || CurrentWaypoint <= 0)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(PathHandle);

	//웨이포인트 뒤로 이동
	MoveWaypointTarget(CurrentWaypoint-1);
}

void AAWaypointManager::SuccessEndGame()
{
	GetWorld()->GetTimerManager().ClearTimer(PathHandle);
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

void AAWaypointManager::MoveWaypointTarget(int32 InWaypoint)
{
	//현재 Waypoint 갱신
	CurrentWaypoint = InWaypoint;

	//적이 Waypoint 이동
	if (CurrentTarget)
	{
		CurrentTarget->SetActorLocation(Path[InWaypoint]->GetActorLocation());
		CurrentTarget->SetActorRotation(Path[InWaypoint]->GetActorRotation());
		CurrentTarget->SetState(EStateEnemy::Waiting);
	}

	//마지막 웨이포인트에 도착했다면 끝을 알림
	if (CurrentWaypoint == WaypointInfo.Num() - 1)
	{
		if (EnemyReachEndPointMutiDelegate.IsBound())
		{
			EnemyReachEndPointMutiDelegate.Broadcast();
		}
	}
	//n 초 뒤 다음 Waypoint 로 이동 
	else if (Path.Num() > InWaypoint + 1 && WaypointInfo.Num() > InWaypoint)
	{
		GetWorld()->GetTimerManager().SetTimer(PathHandle, this, &AAWaypointManager::ArrivedOnHGWaypoint, WaypointInfo[InWaypoint], false);
	}
	else if (Path.Num() > InWaypoint + 1 && WaypointInfo.Num() <= InWaypoint)
	{
		UE_LOG(LogTemp, Error, TEXT("데이터테이블의 WaitTime 보다 Waypoint가 많아서 누락된 Waypoint가 있습니다."));
	}
}

//Waypoint 에 도착했을 때 실행하는 함수
void AAWaypointManager::ArrivedOnHGWaypoint()
{
	GetWorld()->GetTimerManager().ClearTimer(PathHandle);
	
	//적이 Waypoint로 이동
	MoveWaypointTarget(CurrentWaypoint + 1);
}

//Day가 넘어갈때 초기화 해줄 함수
void AAWaypointManager::ResetWaypoint()
{
	CurrentWaypoint = 0;
	//Day에 해당하는 몬스터 적용
	if (IHPMinigameDataInterface* gs = Cast<IHPMinigameDataInterface>(GetWorld()->GetGameState()))
	{
		if (TargetArr.IsValidIndex(gs->GetCurrentDay() - 1))
		{
			CurrentTarget->SetActorHiddenInGame(true);
			CurrentTarget = TargetArr[gs->GetCurrentDay() - 1];
			CurrentTarget->SetActorHiddenInGame(false);
		}
	}
	//처음 웨이포인트부터 출발
	GetWorld()->GetTimerManager().SetTimer(PathHandle, this, &AAWaypointManager::ArrivedOnHGWaypoint, WaypointInfo[0], false);
}
