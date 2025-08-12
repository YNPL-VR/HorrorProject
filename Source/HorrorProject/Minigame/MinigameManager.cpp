// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/MinigameManager.h"
#include "Actor/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"
#include "BalloonSpawnPoint.h"
#include "Balloon.h"
#include <Game/HPGameStateBase.h>

#define MAXBALLOON 20

// Sets default values
AMinigameManager::AMinigameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<AWeapon> GunClassFinder(TEXT("/Game/LSJ/Blueprint/BP_Pistol.BP_Pistol_C"));
	if (GunClassFinder.Succeeded())
	{
		GunClass = GunClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GunClass를 찾을 수 없습니다."));
	}

	//DataTable 초기화
	static ConstructorHelpers::FObjectFinder<UDataTable> MinigameBalloonDataTableFinder(TEXT("/Game/Datatable/NumBalloon.NumBalloon"));
	if (MinigameBalloonDataTableFinder.Succeeded())
	{
		MinigameBalloonDataTable = MinigameBalloonDataTableFinder.Object;
		UE_LOG(LogTemp, Warning, TEXT("MinigameBalloonDataTable Succeed!"));
	}
}

// Called when the game starts or when spawned
void AMinigameManager::BeginPlay()
{
	Super::BeginPlay();

	//데이터테이블에서 MinigameBalloonData 가져오기
	const FString ContextString(TEXT(" AMinigameManager::BeginPlay"));
	MinigameBalloonDataTable->GetAllRows(ContextString, MinigameBalloonData);


	CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(GunClass, GetActorLocation(), GetActorRotation());
	if(nullptr!=CurrentWeapon)
	{
		CurrentWeapon->CatchWeaponDynamicMultiDelegate.AddDynamic(this, &AMinigameManager::OffSpawnWeaponTimer);
		CurrentWeapon->PutWeaponDynamicMultiDelegate.AddDynamic(this, &AMinigameManager::OnSpawnWeaponTimer);
		AllWeapons.Add(CurrentWeapon);
	}

	UWorld* World = GetWorld();
	if (World)
	{
		//풍선 스폰 위치 액터 찾기
		for (TActorIterator<ABalloonSpawnPoint> It(World); It; ++It)
		{
			ABalloonSpawnPoint* BalloonSpawnPoint = *It;
			if (BalloonSpawnPoint)
			{
				BalloonSpawnPoints.Add(BalloonSpawnPoint);
			}
		}
	}
	for (int32 BalloonSpawnCount = 0; BalloonSpawnCount < MAXBALLOON; ++BalloonSpawnCount)
	{
		ABalloon* Balloon = GetWorld()->SpawnActor<ABalloon>(ABalloon::StaticClass());
		Balloon->HitBalloonWithWeapon.BindUFunction(this,FName("CheckCorrectBalloon"));
		Balloon->AddToRoot();
		Balloon->DeactivateToSave();
		BalloonQueue.Enqueue(Balloon);
	}
	
	
}

void AMinigameManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	

	while (!BalloonQueue.IsEmpty())
	{
		ABalloon* Balloon;
		BalloonQueue.Dequeue(Balloon);
		Balloon->RemoveFromRoot();
		Balloon->Destroy();
		Balloon = nullptr;
	}

	for (auto& Balloon : UsingBalloons)
	{
		GetWorld()->GetTimerManager().ClearTimer(Balloon->SpawnTimerHandle);
		
		Balloon->Destroy();
		Balloon = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AMinigameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMinigameManager::OnSpawnWeaponTimer()
{
	//미니게임 강제종료
	StopMinigame();

	GetWorld()->GetTimerManager().SetTimer(SpawnWeaponHandle,this, &AMinigameManager::ResetWeapon, SpawnTime,false);
}

void AMinigameManager::OffSpawnWeaponTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnWeaponHandle);
	//미니게임 시작 타이머 작동
	//GetWorld()->GetTimerManager().SetTimer(StartMinigameHandle,this, &AMinigameManager::StartMinigame,1.0f,false);
	StartMinigame();
}

void AMinigameManager::ResetWeapon()
{
	if (nullptr != CurrentWeapon)
	{
		USkeletalMeshComponent* FoundMesh = CurrentWeapon->FindComponentByClass<USkeletalMeshComponent>();
		if (FoundMesh)
		{
			FoundMesh->SetSimulatePhysics(false);
		}
		CurrentWeapon->SetActorLocationAndRotation(GetActorLocation(), GetActorRotation());
		if (FoundMesh)
		{
			FoundMesh->SetSimulatePhysics(true);
		}
	}

	
}

void AMinigameManager::StartMinigame()
{
	//데이터 테이블이 비었다면
	if (MinigameBalloonData.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("MinigameBalloonData Empty"));
		return;
	}
	if (BalloonSpawnPoints.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("BalloonSpawnPoints Empty"));
		return;
	}

	//생성할 풍선 수
	int32 BalloonNum = 0;
	float BalloonSpeed = 0.0f;
	//한 라인에서의 스폰 시간 차이값
	float LineRandomInterval = 0.0f;
	//현재 레벨 받기
	int32 currentMinigameLevel = 0;
	IHPMinigameDataInterface* gs = GetWorld()->GetGameState<IHPMinigameDataInterface>();
	if (gs)
	{
		currentMinigameLevel = gs->GetMinigameLevel();
	}

	switch (CurrentMinigame)
	{
	case EMinigame::NumBalloon:
		//난이도가 데이터베이스의 항목보다 많다면 //예외처리
		if (MinigameBalloonData.Num() - 1 < currentMinigameLevel)
		{
			UE_LOG(LogTemp, Error, TEXT("MinigameBalloonData.Num() - 1 < currentMinigameLevel"));
			currentMinigameLevel = MinigameBalloonData.Num() - 1;
		}

		//레벨에 맞춰서 데이터 테이블에서 정보 가져오기
		BalloonNum = MinigameBalloonData[currentMinigameLevel]->BalloonNum;
		BalloonSpeed = MinigameBalloonData[currentMinigameLevel]->BalloonSpeed;
		LineRandomInterval = MinigameBalloonData[currentMinigameLevel]->LineRandomInterval;
		break;
	}

	//풍선 수만큼 생성
	int32 Count = 0;
	int32 MaxCount = BalloonNum;
	while (BalloonNum > 0)
	{
		//랜덤 스폰위치 결정
		if (Count % BalloonSpawnPoints.Num() == 0)
		{
			int ShowNumber = Count;

			//숫자 순서 대입
			for (const auto& BalloonSpawner : BalloonSpawnPoints)
			{
				BalloonSpawner->ScreenBalloonNumber = ++ShowNumber;
			}
			//랜덤으로 순서 섞기
			for (int32 BalloonPointCount = BalloonSpawnPoints.Num() - 1; BalloonPointCount > 0; --BalloonPointCount)
			{
				int32 RandIdx = FMath::RandRange(0, BalloonSpawnPoints.Num() - 1);
				BalloonSpawnPoints.Swap(BalloonPointCount, RandIdx);
			}
		}
		//속도로 풍선 길이만큼 지난 시간 예측하고 생성 - 풍선이 전부 같은 속도이므로 랜덤시간 + 속도로 풍선 길이만큼 지난 시간
		for (const auto& BalloonSpawner : BalloonSpawnPoints)
		{
			if (BalloonNum <= 0)
			{
				break;
			}

			if (MaxCount < BalloonSpawner->ScreenBalloonNumber)
			{
				continue;
			}

			//큐에서 풍선 꺼내기
			ABalloon* Balloon;
			BalloonQueue.Dequeue(Balloon);
			Balloon->RemoveFromRoot();
			//사용중인 목록에 추가
			UsingBalloons.Add(Balloon);
			//시간예측
			int Z = Balloon->GetActorMesh()->GetBounds().GetBox().GetSize().Z;
			//속도로 풍선 길이만큼 지나는 예측 시간 
			float WaitTime = (Z/2) / BalloonSpeed;
			float RandomInterVal = FMath::RandRange(0.0f, LineRandomInterval);
			//누적해야한다. 이전 스폰 시간을 가져와서 현재 스폰시간에 더해준다.
			float CurrentBalloonSpawnTime = BalloonSpawner->PreviousSpawnTime+ WaitTime + RandomInterVal;
			BalloonSpawner->PreviousSpawnTime = CurrentBalloonSpawnTime;

			//랜덤시간 , 어떻게 1줄에 랜덤번호를 넣지? - BalloonSpawnPoints
			// Todo : 어떻게 타이머에 인자를 받지? ABalloonSpawnPoint , Balloon
			FTimerDelegate SpawnTimerDelegate;
			SpawnTimerDelegate.BindUFunction(this, FName("SpawnBalloon"), BalloonSpawner->GetActorLocation(), BalloonSpawner->GetActorRotation(), BalloonSpeed,
				BalloonSpawner->ScreenBalloonNumber, Balloon);
			GetWorld()->GetTimerManager().SetTimer(Balloon->SpawnTimerHandle, SpawnTimerDelegate, CurrentBalloonSpawnTime, false);
		
			//해당 위치에 스폰
			//게임에 따라 매터리얼 바꾸기
			//데이터베이스에서 받아서 시간차 스폰 - 타이머

			--BalloonNum;
			++Count;
		}
	}
	

	
}

void AMinigameManager::StopMinigame()
{
	DeactivateAllBalloon();
}

void AMinigameManager::SpawnBalloon(FVector Location, FRotator Rotation, float Speed, int32 Number, ABalloon* Balloon)
{
	switch (CurrentMinigame)
	{
	case EMinigame::NumBalloon:
		Balloon->SetNumberWidgetVisible(true);
		break;
	}
	
	Balloon->ActivateToUse(Location, Rotation, Speed*10.0f);
	Balloon->SetNumberInWidget(Number);
}
//사용중인 풍선 전부 비활성화
void AMinigameManager::DeactivateAllBalloon()
{
	//풍선 비활성화
	for (auto& Balloon : UsingBalloons)
	{
		Balloon->SetNumberWidgetVisible(false);
		Balloon->DeactivateToSave();
		GetWorld()->GetTimerManager().ClearTimer(Balloon->SpawnTimerHandle);
		BalloonQueue.Enqueue(Balloon);
	}

	//값 초기화
	for (const auto& BalloonSpawner : BalloonSpawnPoints)
	{
		BalloonSpawner->ScreenBalloonNumber = 0;
		BalloonSpawner->PreviousSpawnTime = 0.0f;
	}
}

void AMinigameManager::CheckCorrectBalloon(class ABalloon* Balloon)
{
	switch (CurrentMinigame)
	{
	case EMinigame::NumBalloon:

		break;
	}
}

