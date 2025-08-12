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
		UE_LOG(LogTemp, Error, TEXT("GunClass�� ã�� �� �����ϴ�."));
	}

	//DataTable �ʱ�ȭ
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

	//���������̺��� MinigameBalloonData ��������
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
		//ǳ�� ���� ��ġ ���� ã��
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
	//�̴ϰ��� ��������
	StopMinigame();

	GetWorld()->GetTimerManager().SetTimer(SpawnWeaponHandle,this, &AMinigameManager::ResetWeapon, SpawnTime,false);
}

void AMinigameManager::OffSpawnWeaponTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnWeaponHandle);
	//�̴ϰ��� ���� Ÿ�̸� �۵�
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
	//������ ���̺��� ����ٸ�
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

	//������ ǳ�� ��
	int32 BalloonNum = 0;
	float BalloonSpeed = 0.0f;
	//�� ���ο����� ���� �ð� ���̰�
	float LineRandomInterval = 0.0f;
	//���� ���� �ޱ�
	int32 currentMinigameLevel = 0;
	IHPMinigameDataInterface* gs = GetWorld()->GetGameState<IHPMinigameDataInterface>();
	if (gs)
	{
		currentMinigameLevel = gs->GetMinigameLevel();
	}

	switch (CurrentMinigame)
	{
	case EMinigame::NumBalloon:
		//���̵��� �����ͺ��̽��� �׸񺸴� ���ٸ� //����ó��
		if (MinigameBalloonData.Num() - 1 < currentMinigameLevel)
		{
			UE_LOG(LogTemp, Error, TEXT("MinigameBalloonData.Num() - 1 < currentMinigameLevel"));
			currentMinigameLevel = MinigameBalloonData.Num() - 1;
		}

		//������ ���缭 ������ ���̺��� ���� ��������
		BalloonNum = MinigameBalloonData[currentMinigameLevel]->BalloonNum;
		BalloonSpeed = MinigameBalloonData[currentMinigameLevel]->BalloonSpeed;
		LineRandomInterval = MinigameBalloonData[currentMinigameLevel]->LineRandomInterval;
		break;
	}

	//ǳ�� ����ŭ ����
	int32 Count = 0;
	int32 MaxCount = BalloonNum;
	while (BalloonNum > 0)
	{
		//���� ������ġ ����
		if (Count % BalloonSpawnPoints.Num() == 0)
		{
			int ShowNumber = Count;

			//���� ���� ����
			for (const auto& BalloonSpawner : BalloonSpawnPoints)
			{
				BalloonSpawner->ScreenBalloonNumber = ++ShowNumber;
			}
			//�������� ���� ����
			for (int32 BalloonPointCount = BalloonSpawnPoints.Num() - 1; BalloonPointCount > 0; --BalloonPointCount)
			{
				int32 RandIdx = FMath::RandRange(0, BalloonSpawnPoints.Num() - 1);
				BalloonSpawnPoints.Swap(BalloonPointCount, RandIdx);
			}
		}
		//�ӵ��� ǳ�� ���̸�ŭ ���� �ð� �����ϰ� ���� - ǳ���� ���� ���� �ӵ��̹Ƿ� �����ð� + �ӵ��� ǳ�� ���̸�ŭ ���� �ð�
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

			//ť���� ǳ�� ������
			ABalloon* Balloon;
			BalloonQueue.Dequeue(Balloon);
			Balloon->RemoveFromRoot();
			//������� ��Ͽ� �߰�
			UsingBalloons.Add(Balloon);
			//�ð�����
			int Z = Balloon->GetActorMesh()->GetBounds().GetBox().GetSize().Z;
			//�ӵ��� ǳ�� ���̸�ŭ ������ ���� �ð� 
			float WaitTime = (Z/2) / BalloonSpeed;
			float RandomInterVal = FMath::RandRange(0.0f, LineRandomInterval);
			//�����ؾ��Ѵ�. ���� ���� �ð��� �����ͼ� ���� �����ð��� �����ش�.
			float CurrentBalloonSpawnTime = BalloonSpawner->PreviousSpawnTime+ WaitTime + RandomInterVal;
			BalloonSpawner->PreviousSpawnTime = CurrentBalloonSpawnTime;

			//�����ð� , ��� 1�ٿ� ������ȣ�� ����? - BalloonSpawnPoints
			// Todo : ��� Ÿ�̸ӿ� ���ڸ� ����? ABalloonSpawnPoint , Balloon
			FTimerDelegate SpawnTimerDelegate;
			SpawnTimerDelegate.BindUFunction(this, FName("SpawnBalloon"), BalloonSpawner->GetActorLocation(), BalloonSpawner->GetActorRotation(), BalloonSpeed,
				BalloonSpawner->ScreenBalloonNumber, Balloon);
			GetWorld()->GetTimerManager().SetTimer(Balloon->SpawnTimerHandle, SpawnTimerDelegate, CurrentBalloonSpawnTime, false);
		
			//�ش� ��ġ�� ����
			//���ӿ� ���� ���͸��� �ٲٱ�
			//�����ͺ��̽����� �޾Ƽ� �ð��� ���� - Ÿ�̸�

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
//������� ǳ�� ���� ��Ȱ��ȭ
void AMinigameManager::DeactivateAllBalloon()
{
	//ǳ�� ��Ȱ��ȭ
	for (auto& Balloon : UsingBalloons)
	{
		Balloon->SetNumberWidgetVisible(false);
		Balloon->DeactivateToSave();
		GetWorld()->GetTimerManager().ClearTimer(Balloon->SpawnTimerHandle);
		BalloonQueue.Enqueue(Balloon);
	}

	//�� �ʱ�ȭ
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

