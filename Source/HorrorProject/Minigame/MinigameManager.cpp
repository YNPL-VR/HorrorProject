// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/MinigameManager.h"
#include "Actor/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"
#include "BalloonSpawnPoint.h"
#include "Balloon.h"

#define MAXBALLOON 20

// Sets default values
AMinigameManager::AMinigameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<AWeapon> GunClassFinder(TEXT("/Game/LSJ/Blueprint/Pistol.Pistol_C"));
	if (GunClassFinder.Succeeded())
	{
		GunClass = GunClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GunClass를 찾을 수 없습니다."));
	}
}

// Called when the game starts or when spawned
void AMinigameManager::BeginPlay()
{
	Super::BeginPlay();

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
		Balloon->AddToRoot();
		Balloon->DeactivateToSave();
		Balloons.Enqueue(Balloon);
	}
	
	
}

void AMinigameManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	while (!Balloons.IsEmpty())
	{
		TObjectPtr<ABalloon> Balloon;
		Balloons.Dequeue(Balloon);
		delete Balloon;
		Balloon = nullptr;
	}
}

// Called every frame
void AMinigameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMinigameManager::OnSpawnWeaponTimer()
{
	GetWorld()->GetTimerManager().SetTimer(SpawnWeaponHandle,this, &AMinigameManager::ResetWeapon, SpawnTime,false);
}

void AMinigameManager::OffSpawnWeaponTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnWeaponHandle);
	//미니게임 시작 타이머 작동
	GetWorld()->GetTimerManager().SetTimer(StartMinigameHandle,this, &AMinigameManager::StartMinigame,1.0f,false);
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

	//미니게임 강제종료
	StopMinigame();
}

void AMinigameManager::StartMinigame()
{

	//Todo : 랜덤 선택하여 스폰위치 결정 / 어떻게 나머지를 고르지?
	int32 RandomIdx = FMath::RandRange(0, BalloonSpawnPoints.Num() - 1);
	

	//Todo : 오브젝트풀링활용 꺼내서 쓰기
	TObjectPtr<ABalloon> Balloon;
	Balloons.Dequeue(Balloon);
	Balloon->RemoveFromRoot();
}

void AMinigameManager::StopMinigame()
{
	
}

