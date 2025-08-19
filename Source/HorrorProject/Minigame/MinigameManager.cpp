// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/MinigameManager.h"
#include "Actor/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"
#include "BalloonSpawnPoint.h"
#include "Balloon.h"
#include "Game/HPGameStateBase.h"
#include "Player/HPPawn.h"
#include "Kismet/GameplayStatics.h"

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
	//풍선 오브젝트 풀 채우기
	for (int32 BalloonSpawnCount = 0; BalloonSpawnCount < MAXBALLOON; ++BalloonSpawnCount)
	{
		ABalloon* Balloon = GetWorld()->SpawnActor<ABalloon>(ABalloon::StaticClass());
		Balloon->HitBalloonWithWeapon.BindUFunction(this,FName("CheckCorrectBalloon"));
		Balloon->AddToRoot();
		Balloon->DeactivateToSave();
		BalloonQueue.Enqueue(Balloon);
	}
	
	if (IHPMinigameDataInterface* gs = Cast<IHPMinigameDataInterface>(GetWorld()->GetGameState()))
	{
		//다음날 변경시 미니게임 바꾸기
		gs->BeginNextDayMultiDelegate.AddDynamic(this, &AMinigameManager::SetMinigame);
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
		Balloon->RemoveFromRoot();
		Balloon->Destroy();
	}
	UsingBalloons.Empty();
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
	//미니게임 시작
	StartMinigame();
}
//Todo : 무기마다 리셋 설정 다를 예정
void AMinigameManager::ResetWeapon()
{
	if (nullptr != CurrentWeapon)
	{
		USkeletalMeshComponent* FoundMesh = CurrentWeapon->FindComponentByClass<USkeletalMeshComponent>();
		if (FoundMesh)
		{
			GetWorld()->GetTimerManager().ClearTimer(SpawnWeaponHandle);
			FoundMesh->SetSimulatePhysics(false);
			CurrentWeapon->SetActorLocationAndRotation(GetActorLocation(), GetActorRotation());
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
	CurrentMinigameLevel = 0;
	IHPMinigameDataInterface* gs = GetWorld()->GetGameState<IHPMinigameDataInterface>();
	if (gs)
	{
		CurrentMinigameLevel = gs->GetMinigameLevel();
	}

	switch (CurrentMinigame)
	{
	case EMinigame::NumBalloon:
		//난이도가 데이터베이스의 항목보다 많다면 //예외처리
		if (MinigameBalloonData.Num() - 1 < CurrentMinigameLevel)
		{
			UE_LOG(LogTemp, Error, TEXT("MinigameBalloonData.Num() - 1 < currentMinigameLevel"));
			CurrentMinigameLevel = MinigameBalloonData.Num() - 1;
		}
		//풍선을 맞췄을때 시작 숫자
		CorrectBalloonNumber = 1;
		//레벨에 맞춰서 데이터 테이블에서 정보 가져오기
		BalloonNum = MinigameBalloonData[CurrentMinigameLevel]->BalloonNum;
		BalloonSpeed = MinigameBalloonData[CurrentMinigameLevel]->BalloonSpeed;
		LineRandomInterval = MinigameBalloonData[CurrentMinigameLevel]->LineRandomInterval;
		break;
	}

	//풍선 수만큼 생성
	int32 Count = 0;
	int32 MaxCount = BalloonNum;
	while (BalloonNum > 0)
	{
		//랜덤 스폰위치 결정
		//n번째 마다 BalloonSpawnPoints의 정보 갱신
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
			
			//사용중인 목록에 추가
			UsingBalloons.Add(Balloon);
			//풍선의 높이 길이
			int ZSize = Balloon->GetActorMesh()->GetBounds().GetBox().GetSize().Z;
			//속도로 풍선 길이만큼 지나는 예측 시간 
			float WaitTime = (ZSize/2) / BalloonSpeed;
			float RandomInterVal = FMath::RandRange(0.0f, LineRandomInterval);
			//누적해야한다. 이전 스폰 시간을 가져와서 현재 스폰시간에 더해준다.
			float CurrentBalloonSpawnTime = BalloonSpawner->PreviousSpawnTime+ WaitTime + RandomInterVal;
			BalloonSpawner->PreviousSpawnTime = CurrentBalloonSpawnTime;
	
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
	Balloon->RemoveFromRoot();
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
		//마지막 숫자라면 배터리 충전
		if (CorrectBalloonNumber == MinigameBalloonData[CurrentMinigameLevel]->BalloonNum)
		{
			//플레이어 찾기
			APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			if (AHPPawn* Player = Cast<AHPPawn>(PlayerPawn))
			{
				Player->ChargeBattery(CurrentMinigameLevel);
			}

			//풍선 비활성화 후 풀에 넣기
			Balloon->AddToRoot();
			Balloon->DeactivateToSave();
			BalloonQueue.Enqueue(Balloon);

			//ResetWeapon();
		}
		//알맞은 숫자를 맞췄다면 패스
		else if (CorrectBalloonNumber == Balloon->GetNumberInWidget())
		{
			++CorrectBalloonNumber;

			//풍선 비활성화 후 풀에 넣기
			Balloon->AddToRoot();
			Balloon->DeactivateToSave();
			BalloonQueue.Enqueue(Balloon);
			return;
		}
		
		//틀린 숫자 or 마지막 풍선을 맞췄다면 게임종료 
		StopMinigame();
		//다시 게임 시작
		StartMinigame();
		break;
	}
}

void AMinigameManager::SetMinigame()
{
	//진행중인 게임 중단
	StopMinigame();
	if (IHPMinigameDataInterface* gs = Cast<IHPMinigameDataInterface>(GetWorld()->GetGameState()))
	{
		//미니게임 바꾸기
		CurrentMinigame = (EMinigame)gs->GetCurrentDay();
	}
	
}

