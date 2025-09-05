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
#include "Actor/ColorDisplayActor.h"
#include <Components/BoxComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

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
	static ConstructorHelpers::FClassFinder<AWeapon> DartClassFinder(TEXT("/Game/LSJ/Blueprint/BP_Dart.BP_Dart_C"));
	if (DartClassFinder.Succeeded())
	{
		DartClass = DartClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DartClass를 찾을 수 없습니다."));
	}
	static ConstructorHelpers::FClassFinder<AWeapon> BatClassFinder(TEXT("/Game/LSJ/Blueprint/BP_Bat.BP_Bat_C"));
	if (BatClassFinder.Succeeded())
	{
		BatClass = BatClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BatClass를 찾을 수 없습니다."));
	}

	//NumberBalloon DataTable 초기화
	static ConstructorHelpers::FObjectFinder<UDataTable> MinigameBalloonDataTableFinder(TEXT("/Game/Datatable/NumBalloon.NumBalloon"));
	if (MinigameBalloonDataTableFinder.Succeeded())
	{
		MinigameBalloonDataTable = MinigameBalloonDataTableFinder.Object;
	}
	//ColorBalloon DataTable 초기화
	static ConstructorHelpers::FObjectFinder<UDataTable> ColorBalloonDataTableFinder(TEXT("/Script/Engine.DataTable'/Game/Datatable/ColorBalloon.ColorBalloon'"));
	if (ColorBalloonDataTableFinder.Succeeded())
	{
		ColorBalloonDataTable = ColorBalloonDataTableFinder.Object;
	}
	//Color DataTable 초기화
	static ConstructorHelpers::FObjectFinder<UDataTable> ColorDataTableFinder(TEXT("/Game/Datatable/SelectedColor.SelectedColor"));
	if (ColorDataTableFinder.Succeeded())
	{
		SelectedColorDataTable = ColorDataTableFinder.Object;
	}
	//DartBalloon DataTable 초기화
	static ConstructorHelpers::FObjectFinder<UDataTable> DartBalloonDataTableFinder(TEXT("/Script/Engine.DataTable'/Game/Datatable/DartBalloon.DartBalloon'"));
	if (DartBalloonDataTableFinder.Succeeded())
	{
		DartBalloonDataTable = DartBalloonDataTableFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> BatBalloonDataTableFinder(TEXT("/Script/Engine.DataTable'/Game/Datatable/BatBalloon.BatBalloon'"));
	if (BatBalloonDataTableFinder.Succeeded())
	{
		BatBalloonDataTable = BatBalloonDataTableFinder.Object;
	}
}

// Called when the game starts or when spawned
void AMinigameManager::BeginPlay()
{
	Super::BeginPlay();

	//데이터테이블에서 MinigameBalloonData 가져오기
	const FString ContextString(TEXT(" AMinigameManager::BeginPlay"));
	MinigameBalloonDataTable->GetAllRows(ContextString, MinigameBalloonData);
	ColorBalloonDataTable->GetAllRows(ContextString, ColorBalloonData);
	SelectedColorDataTable->GetAllRows(ContextString, SelectedColorData);
	DartBalloonDataTable->GetAllRows(ContextString, DartBalloonData);
	BatBalloonDataTable->GetAllRows(ContextString, BatBalloonData);

	//총 무기 스폰
	CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(GunClass, GetActorLocation(), GetActorRotation());
	if(nullptr!=CurrentWeapon)
	{
		//안보이는 경우 충돌방지
		USkeletalMeshComponent* FoundMesh = CurrentWeapon->FindComponentByClass<USkeletalMeshComponent>();
		if (FoundMesh)
		{
			FoundMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
			FoundMesh->SetCollisionObjectType(ECC_GameTraceChannel4);
		}
		CurrentWeapon->CatchWeaponDynamicMultiDelegate.AddDynamic(this, &AMinigameManager::OffSpawnWeaponTimer);
		CurrentWeapon->PutWeaponDynamicMultiDelegate.AddDynamic(this, &AMinigameManager::OnSpawnWeaponTimer);
		AllWeapons.Add(CurrentWeapon);
		CurrentWeapon->SetActorHiddenInGame(true);
		//배트 무기 스폰
		CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(BatClass, GetActorLocation(), GetActorRotation());
		UBoxComponent* FoundBox = CurrentWeapon->FindComponentByClass<UBoxComponent>();
		if (FoundBox)
		{
			FoundBox->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
			FoundBox->SetCollisionObjectType(ECC_GameTraceChannel4);
		}
		CurrentWeapon->CatchWeaponDynamicMultiDelegate.AddDynamic(this, &AMinigameManager::OffSpawnWeaponTimer);
		CurrentWeapon->PutWeaponDynamicMultiDelegate.AddDynamic(this, &AMinigameManager::OnSpawnWeaponTimer);
		AllWeapons.Add(CurrentWeapon);
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	UWorld* World = GetWorld();
	if (World)
	{
		// TV 액터 찾기
		for (TActorIterator<AColorDisplayActor> It(World); It; ++It)
		{
			AColorDisplayActor* ColorDisplayActor = *It;
			if (ColorDisplayActor)
			{
				TVActor = ColorDisplayActor;
			}
		}
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
		//Todo : Bat 테스트중
		CurrentMinigame = EMinigame::BatBall;//static_cast<EMinigame>(gs->GetCurrentDay() - 1);
		SwapWeapon(CurrentMinigame);

		//SwapWeapon(static_cast<EMinigame>(gs->GetCurrentDay() - 1));
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
//무기를 드랍했을때 실행되는 함수
void AMinigameManager::OnSpawnWeaponTimer()
{
	//무기가 존재하지 않으면 리턴
	if (AllWeapons.Num() < 1)
		return;
	if ((CurrentMinigame==EMinigame::NumBalloon && CurrentWeapon->WeaponType == EWeaponType::Gun) || (CurrentMinigame == EMinigame::BatBall && CurrentWeapon->WeaponType == EWeaponType::Bat))
	{
		//미니게임 강제종료
		StopMinigame();
		//무기 스폰
		GetWorld()->GetTimerManager().SetTimer(SpawnWeaponHandle, this, &AMinigameManager::ResetWeapon, SpawnTime, false);
	}
	else if (CurrentMinigame == EMinigame::DartBalloon && CurrentWeapon->WeaponType == EWeaponType::Dart)
	{
		//일정시간동안 들지 않았을때 미니 게임 종료
		GetWorld()->GetTimerManager().SetTimer(SpawnWeaponHandle, this, &AMinigameManager::StopMinigame, 10.0f, false);
		//무기스폰위치에 새로운 다트 생성
		CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(DartClass, GetActorLocation(), GetActorRotation());
		CurrentWeapon->CatchWeaponDynamicMultiDelegate.AddDynamic(this, &AMinigameManager::OffSpawnWeaponTimer);
		CurrentWeapon->PutWeaponDynamicMultiDelegate.AddDynamic(this, &AMinigameManager::OnSpawnWeaponTimer);

	}
	
}
//Todo : 플레이어에서 해야하지 않을까?
//무기를 들었을때 실행되는 함수
void AMinigameManager::OffSpawnWeaponTimer(AWeapon* Weapon)
{
	//무기가 존재하지 않으면 리턴
	if (AllWeapons.Num() < 1)
		return;
	//총이라면 들었을때 게임시작
	if (Weapon->WeaponType == EWeaponType::Gun || Weapon->WeaponType == EWeaponType::Bat)
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnWeaponHandle);
		//미니게임 시작
		StartMinigame();
	}
	//다트를 들었을때 게임중이 아니라면 게임 시작
	else if (Weapon->WeaponType == EWeaponType::Dart)
	{
		//Destroy 해제 -> 문제발생 댕글리포인터가 됨 //->드랍했을때 Destroy타이머를 작동하면 해결됨
		GetWorld()->GetTimerManager().ClearTimer(Weapon->SpawnWeaponHandle);
		//미니 게임 종료 타이머 해제
		GetWorld()->GetTimerManager().ClearTimer(SpawnWeaponHandle);
		if(UsingBalloons.IsEmpty())
			StartMinigame();
	}
}
//Todo : 무기마다 리셋 설정 다를 예정
void AMinigameManager::ResetWeapon()
{
	if (CurrentWeapon->WeaponType == EWeaponType::Gun)
	{
		USkeletalMeshComponent* FoundMesh = CurrentWeapon->FindComponentByClass<USkeletalMeshComponent>();
		if (FoundMesh)
		{
			FoundMesh->SetSimulatePhysics(false);
			CurrentWeapon->SetActorLocationAndRotation(GetActorLocation(), GetActorRotation());
			FoundMesh->SetSimulatePhysics(true);
		}
	}
	else if (CurrentWeapon->WeaponType == EWeaponType::Dart)
	{
		CurrentWeapon->SetActorLocationAndRotation(GetActorLocation(), GetActorRotation());
	}
	else if (CurrentWeapon->WeaponType == EWeaponType::Bat)
	{
		UBoxComponent* FoundBox = CurrentWeapon->FindComponentByClass<UBoxComponent>();
		if (FoundBox)
		{
			FoundBox->SetSimulatePhysics(false);
			CurrentWeapon->SetActorLocationAndRotation(GetActorLocation(), GetActorRotation());
			FoundBox->SetSimulatePhysics(true);
		}
	}
	//현재 다트를 10초뒤에 제거하고
	//다트를 무기 스폰 위치에 새로 스폰하여 CurrentWeapon 초기화
	
}

void AMinigameManager::StartMinigame()
{
	//데이터 테이블이 비었다면 오류발생 로그
	if (MinigameBalloonData.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("MinigameBalloonData Empty"));
		return;
	}
	//풍선 스폰 포인트가 없다면 오류발생 로그
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
		{
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

			{
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
						Balloon->SetNumberInWidget(BalloonSpawner->ScreenBalloonNumber);
						//풍선의 높이 길이
						int ZSize = Balloon->GetActorMesh()->GetBounds().GetBox().GetSize().Z;
						//속도로 풍선 길이만큼 지나는 예측 시간 
						float WaitTime = (ZSize / 2) / BalloonSpeed;
						float RandomInterVal = FMath::RandRange(0.0f, LineRandomInterval);
						//누적해야한다. 이전 스폰 시간을 가져와서 현재 스폰시간에 더해준다.
						float CurrentBalloonSpawnTime = BalloonSpawner->PreviousSpawnTime + WaitTime + RandomInterVal;
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

			break;
		}
	case EMinigame::ColorBalloon:
	{
		if (TVActor == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("TVActor == nullptr"));
			return;
		}
		if (SelectedColorData.Num() < 1 || ColorBalloonData.Num() < 1)
		{
			UE_LOG(LogTemp, Error, TEXT("SelectedColorData.Num() < 1 || ColorBalloonData.Num() < 1"));
			return;
		}
		//난이도가 데이터베이스의 항목보다 많다면 //예외처리
		if (ColorBalloonData.Num() - 1 < CurrentMinigameLevel)
		{
			UE_LOG(LogTemp, Error, TEXT("ColorBalloonData.Num() - 1 < currentMinigameLevel"));
			CurrentMinigameLevel = ColorBalloonData.Num() - 1;
		}
		//풍선을 맞췄을때 시작 색깔 인덱스
		CorrectBalloonNumber = 0;
		MatchingBalloonOrder.Empty();
		//레벨에 맞춰서 데이터 테이블에서 정보 가져오기
		BalloonNum = ColorBalloonData[CurrentMinigameLevel]->ColorNum;
		BalloonSpeed = ColorBalloonData[CurrentMinigameLevel]->BallonSpeed;
		LineRandomInterval = ColorBalloonData[CurrentMinigameLevel]->LineRandomInterval;
		float ShowColorInterval = ColorBalloonData[CurrentMinigameLevel]->ColorChangeSpeed;

		//ColorNum 만큼 TV에 색 보여주기 -> 다 보여준 후 풍선 스폰
		//ColorNum 만큼 랜덤으로 색을 선택 후 TVActor TArray에 추가
		//같은 색이 나와도 된다.
		//시간 누적
		//처음 시작 시간 TVActor->StartShowColor에서 추가해주었음
		float LastRate = 1.0f;
		//색상 정보 초기화
		TVActor->ClearShowColorList();
		for (int32 Count = 0; Count < BalloonNum; ++Count)
		{
			//색 랜덤 선택
			int32 RandomColorIdx = FMath::RandRange(0, SelectedColorData.Num() - 1);
			MatchingBalloonOrder.Emplace(RandomColorIdx);
			FSelectedColor* BalloonColor = SelectedColorData[RandomColorIdx];
			FLinearColor SelectedColor = FLinearColor(BalloonColor->R, BalloonColor->G, BalloonColor->B, 1.0f);
			//색 추가
			TVActor->AddShowColor(0.1f, SelectedColor);
			//ShowColorInterval 초간 깜박임 효과 //보여줄색 -> 투명으로 보여줄 색 확실하게 구분, 같은색이 나올 수 있다.
			TVActor->AddShowColor(ShowColorInterval, FLinearColor(1, 1, 1, 1));
			LastRate += 0.1f + ShowColorInterval;
		}
		//순서대로 보여주기
		TVActor->StartShowColor();
		//TVActor가 여러번 Timer를 써야하는데 어떤 방식으로 하는 것이 좋을까?
		//Timer를 하나로 쓰고 보여준후 다음것 보여줄 세팅 , 큐 사용, 목록을 전달해주어야함 - 필요사항: 초, 색깔,


		//풍선 스폰 시간
		for (auto BalloonSpawnPoint : BalloonSpawnPoints)
		{
			BalloonSpawnPoint->PreviousSpawnTime = LastRate;
		}

		//순서대로 풍선 스폰
		int32 ColorIdx = 0;
		for (int32 Count = 0; Count < BalloonNum;)
		{
			//랜덤으로 BalloonSpawnPoints 순서 섞기
			if (Count % BalloonSpawnPoints.Num() == 0)
			{
				//색깔을 바꾸기 위해 BalloonSpawnPoint에 풍선 색 index 대입
				for (auto BalloonSpawnPoint : BalloonSpawnPoints)
				{
					if (ColorIdx >= BalloonNum)
						BalloonSpawnPoint->ScreenBalloonNumber = -1;
					else
					{
						BalloonSpawnPoint->ScreenBalloonNumber = MatchingBalloonOrder[ColorIdx];
						++ColorIdx;
					}
				}

				for (int32 BalloonPointCount = BalloonSpawnPoints.Num() - 1; BalloonPointCount > 0; --BalloonPointCount)
				{
					int32 RandIdx = FMath::RandRange(0, BalloonSpawnPoints.Num() - 1);
					BalloonSpawnPoints.Swap(BalloonPointCount, RandIdx);
				}
			}

			//풍선 스폰
			for (auto BalloonSpawnPoint : BalloonSpawnPoints)
			{
				if (BalloonSpawnPoint->ScreenBalloonNumber == -1)
				{
					continue;
				}

				//큐에서 풍선 꺼내기
				ABalloon* Balloon;
				BalloonQueue.Dequeue(Balloon);
				//풍선 메시 변경
				FSelectedColor* BalloonColor = SelectedColorData[BalloonSpawnPoint->ScreenBalloonNumber];
				Balloon->SetColor(FLinearColor(BalloonColor->R, BalloonColor->G, BalloonColor->B, 1.0f));
				Balloon->SetNumberInWidget(BalloonSpawnPoint->ScreenBalloonNumber);

				//사용중인 목록에 추가
				UsingBalloons.Add(Balloon);
				//풍선의 높이 길이
				int ZSize = Balloon->GetActorMesh()->GetBounds().GetBox().GetSize().Z;
				//속도로 풍선 길이만큼 지나는 예측 시간 
				float WaitTime = (ZSize / 2) / BalloonSpeed;
				float RandomInterVal = FMath::RandRange(0.0f, LineRandomInterval);
				//누적해야한다. 이전 스폰 시간을 가져와서 현재 스폰시간에 더해준다.
				float CurrentBalloonSpawnTime = BalloonSpawnPoint->PreviousSpawnTime + WaitTime + RandomInterVal;
				BalloonSpawnPoint->PreviousSpawnTime = CurrentBalloonSpawnTime;
				//풍선 스폰 타이머
				GetWorld()->GetTimerManager().SetTimer(Balloon->SpawnTimerHandle,
					[this, BalloonSpawnPoint, Balloon, BalloonSpeed]()
					{
						SpawnBalloon(BalloonSpawnPoint->GetActorLocation(), BalloonSpawnPoint->GetActorRotation(), BalloonSpeed,
							BalloonSpawnPoint->ScreenBalloonNumber, Balloon);
					}, CurrentBalloonSpawnTime, false);
				++Count;
			}
		}
		break;
	}
	case EMinigame::DartBalloon:
	{
		//난이도가 데이터베이스의 항목보다 많다면 //예외처리
		if (DartBalloonData.Num() - 1 < CurrentMinigameLevel)
		{
			UE_LOG(LogTemp, Error, TEXT("MinigameBalloonData.Num() - 1 < currentMinigameLevel"));
			CurrentMinigameLevel = DartBalloonData.Num() - 1;
		}
		//풍선을 맞췄을때 시작 숫자
		CorrectBalloonNumber = 1;
		//레벨에 맞춰서 데이터 테이블에서 정보 가져오기
		BalloonNum = DartBalloonData[CurrentMinigameLevel]->BalloonNum;
		BalloonSpeed = DartBalloonData[CurrentMinigameLevel]->BalloonSpeed;
		LineRandomInterval = DartBalloonData[CurrentMinigameLevel]->LineRandomInterval;
		{
			//풍선 수만큼 생성
			int32 Count = 0;
			while (BalloonNum > Count)
			{
				//랜덤 스폰위치 결정
				//n번째 마다 BalloonSpawnPoints의 정보 갱신
				if (Count % BalloonSpawnPoints.Num() == 0)
				{
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
					// 총 스폰한 수가 최대치를 넘지 않게 만듬
					if (BalloonNum <= Count)
					{
						return;
					}

					//큐에서 풍선 꺼내기
					ABalloon* Balloon;
					BalloonQueue.Dequeue(Balloon);

					//사용중인 목록에 추가
					UsingBalloons.Add(Balloon);
					//풍선의 높이 길이
					int ZSize = Balloon->GetActorMesh()->GetBounds().GetBox().GetSize().Z;
					//속도로 풍선 길이만큼 지나는 예측 시간 
					float WaitTime = (ZSize / 2) / BalloonSpeed;
					float RandomInterVal = FMath::RandRange(0.0f, LineRandomInterval);
					//누적해야한다. 이전 스폰 시간을 가져와서 현재 스폰시간에 더해준다.
					float CurrentBalloonSpawnTime = BalloonSpawner->PreviousSpawnTime + WaitTime + RandomInterVal;
					BalloonSpawner->PreviousSpawnTime = CurrentBalloonSpawnTime;
					//풍선 스폰 타이머
					GetWorld()->GetTimerManager().SetTimer(Balloon->SpawnTimerHandle,
						[this, BalloonSpawner, Balloon, BalloonSpeed]()
						{
							SpawnBalloon(BalloonSpawner->GetActorLocation(), BalloonSpawner->GetActorRotation(), BalloonSpeed,
								BalloonSpawner->ScreenBalloonNumber, Balloon);
						}, CurrentBalloonSpawnTime, false);
					++Count;
				}
			}
		}

		break;
	}
	case EMinigame::BatBall:
	{
		//난이도가 데이터베이스의 항목보다 많다면 //예외처리
		if (BatBalloonData.Num() - 1 < CurrentMinigameLevel)
		{
			UE_LOG(LogTemp, Error, TEXT("MinigameBalloonData.Num() - 1 < currentMinigameLevel"));
			CurrentMinigameLevel = BatBalloonData.Num() - 1;
		}
		//풍선을 맞췄을때 시작 숫자
		CorrectBalloonNumber = 1;
		//레벨에 맞춰서 데이터 테이블에서 정보 가져오기
		BalloonNum = BatBalloonData[CurrentMinigameLevel]->BalloonNum;
		BalloonSpeed = BatBalloonData[CurrentMinigameLevel]->BalloonSpeed;
		LineRandomInterval = BatBalloonData[CurrentMinigameLevel]->LineRandomInterval;
		{
			//풍선 수만큼 생성
			int32 Count = 0;
			while (BalloonNum > Count)
			{
				//랜덤 스폰위치 결정
				//n번째 마다 BalloonSpawnPoints의 정보 갱신
				if (Count % BalloonSpawnPoints.Num() == 0)
				{
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
					// 총 스폰한 수가 최대치를 넘지 않게 만듬
					if (BalloonNum <= Count)
					{
						return;
					}

					//큐에서 풍선 꺼내기
					ABalloon* Balloon;
					BalloonQueue.Dequeue(Balloon);

					//사용중인 목록에 추가
					UsingBalloons.Add(Balloon);
					//풍선의 높이 길이
					int ZSize = Balloon->GetActorMesh()->GetBounds().GetBox().GetSize().Z;
					//속도로 풍선 길이만큼 지나는 예측 시간 
					float WaitTime = (ZSize / 2) / BalloonSpeed;
					float RandomInterVal = FMath::RandRange(0.0f, LineRandomInterval);
					//누적해야한다. 이전 스폰 시간을 가져와서 현재 스폰시간에 더해준다.
					float CurrentBalloonSpawnTime = BalloonSpawner->PreviousSpawnTime + WaitTime + RandomInterVal;
					BalloonSpawner->PreviousSpawnTime = CurrentBalloonSpawnTime;
					//풍선 스폰 타이머
					GetWorld()->GetTimerManager().SetTimer(Balloon->SpawnTimerHandle,
						[this, BalloonSpawner, Balloon, BalloonSpeed]()
						{ //BalloonSpawner->GetActorRotation() //플레이어를 향하게 
							//y와z축만 반영
							//플레이어의 위치 - 풍선의 위치 = 플레이어를 가르키는 벡터
							//해당 벡터의 방향으로 
							APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
							if (PlayerPawn)
							{
								FVector PlayerPos = PlayerPawn->GetActorLocation();
								FVector BalloonPos = BalloonSpawner->GetActorLocation();
								FVector Direction = PlayerPos - BalloonPos;
								Direction.Z = 0;
								BalloonPos.Z = 0;
								FVector PerpendicularVector = Direction ^ BalloonPos;
								FVector PerpendicularDirection = PerpendicularVector.GetSafeNormal();

								FVector ToBackDirection = BalloonSpawner->GetActorForwardVector() * -1.f;
								ToBackDirection.Z = PlayerPawn->GetActorLocation().Z;
								//FRotationMatrix::MakeFromZX(PerpendicularVector, Direction).Rotator()
								SpawnBalloon(BalloonSpawner->GetActorLocation(), BalloonSpawner->GetActorRotation(), BalloonSpeed,
									BalloonSpawner->ScreenBalloonNumber, Balloon);
							}
					
						}, CurrentBalloonSpawnTime, false);
					++Count;
				}
			}
		}

		break;
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
	//Balloon->SetNumberInWidget(Number);
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
	//DeadZone에 맞았다면 실패처리
	if (-1 == Balloon->GetNumberInWidget())
	{
		//틀린 숫자 or 마지막 풍선을 맞췄다면 게임종료 
		StopMinigame();
		//다시 게임 시작
		StartMinigame();
		return;
	}

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
	case EMinigame::ColorBalloon:
		//마지막 숫자라면 배터리 충전 
		if (MatchingBalloonOrder.Last(0) == Balloon->GetNumberInWidget())
		{
			//플레이어 찾기 //배터리 충전
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
		else if (MatchingBalloonOrder[CorrectBalloonNumber] == Balloon->GetNumberInWidget())
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
	case EMinigame::DartBalloon:
		//마지막 숫자라면 배터리 충전
		if (CorrectBalloonNumber == DartBalloonData[CurrentMinigameLevel]->BalloonNum)
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
		//풍선을 맞췄다면 풍선 카운트 증가
		else
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
	//무기 스폰 클리어
	GetWorld()->GetTimerManager().ClearTimer(SpawnWeaponHandle);
	//진행중인 게임 중단
	StopMinigame();
	if (IHPMinigameDataInterface* gs = Cast<IHPMinigameDataInterface>(GetWorld()->GetGameState()))
	{
		//무기 종류 바꾸기
		CurrentMinigame = static_cast<EMinigame>(gs->GetCurrentDay() - 1);
		SwapWeapon(CurrentMinigame);
	}
}

void AMinigameManager::SwapWeapon(EMinigame Minigame)
{
	USkeletalMeshComponent* FoundMesh = AllWeapons[0]->FindComponentByClass<USkeletalMeshComponent>();
	UBoxComponent* FoundBox = AllWeapons[1]->FindComponentByClass<UBoxComponent>();
	//CurrentWeapon이 다트일때 제거되므로 스왑할때 문제가 됨 -> CurrentWeapon을 쓰지 않고 직접 무기 지정
	switch (Minigame)
	{
	case EMinigame::NumBalloon:
	case EMinigame::ColorBalloon:
		//다른 무기 충돌 안되게 만들기
		
		if (FoundBox)
		{
			FoundBox->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
		}
		AllWeapons[1]->SetActorHiddenInGame(true);
		//보이는 경우 충돌가능
		if (FoundMesh)
		{
			FoundMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Block);
		}
		AllWeapons[0]->SetActorHiddenInGame(false);
		CurrentWeapon = AllWeapons[0];
		break;
	case EMinigame::DartBalloon:
		//다른 무기 충돌 안되게 만들기
		if (FoundMesh)
		{
			FoundMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
		}
		AllWeapons[0]->SetActorHiddenInGame(true);
		if (FoundBox)
		{
			FoundBox->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
		}
		AllWeapons[1]->SetActorHiddenInGame(true);

		CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(DartClass, GetActorLocation(), GetActorRotation());
		FoundBox = CurrentWeapon->FindComponentByClass<UBoxComponent>();
		if (FoundBox)
		{
			FoundBox->SetCollisionResponseToChannel(ECC_Visibility,ECR_Ignore);
			FoundBox->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
			FoundBox->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
		
		}
		CurrentWeapon->CatchWeaponDynamicMultiDelegate.AddDynamic(this, &AMinigameManager::OffSpawnWeaponTimer);
		CurrentWeapon->PutWeaponDynamicMultiDelegate.AddDynamic(this, &AMinigameManager::OnSpawnWeaponTimer);
		break;
	case EMinigame::BatBall:
		//다른 무기 충돌 안되게 만들기
		if (FoundMesh)
		{
			FoundMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
		}
		AllWeapons[0]->SetActorHiddenInGame(true);
		//보이는 경우 충돌가능
		if (FoundBox)
		{
			FoundBox->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Block);
		}
		AllWeapons[1]->SetActorHiddenInGame(false);
		CurrentWeapon = AllWeapons[1];
		break;	
	}

	ResetWeapon();
}

void AMinigameManager::RandomBalloonSpawnPoint(int32& ShowNumber)
{
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

