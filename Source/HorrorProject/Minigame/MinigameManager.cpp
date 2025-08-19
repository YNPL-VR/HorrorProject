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
	//ǳ�� ������Ʈ Ǯ ä���
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
		//������ ����� �̴ϰ��� �ٲٱ�
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
	//�̴ϰ��� ��������
	StopMinigame();

	GetWorld()->GetTimerManager().SetTimer(SpawnWeaponHandle,this, &AMinigameManager::ResetWeapon, SpawnTime,false);
}

void AMinigameManager::OffSpawnWeaponTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnWeaponHandle);
	//�̴ϰ��� ����
	StartMinigame();
}
//Todo : ���⸶�� ���� ���� �ٸ� ����
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
	CurrentMinigameLevel = 0;
	IHPMinigameDataInterface* gs = GetWorld()->GetGameState<IHPMinigameDataInterface>();
	if (gs)
	{
		CurrentMinigameLevel = gs->GetMinigameLevel();
	}

	switch (CurrentMinigame)
	{
	case EMinigame::NumBalloon:
		//���̵��� �����ͺ��̽��� �׸񺸴� ���ٸ� //����ó��
		if (MinigameBalloonData.Num() - 1 < CurrentMinigameLevel)
		{
			UE_LOG(LogTemp, Error, TEXT("MinigameBalloonData.Num() - 1 < currentMinigameLevel"));
			CurrentMinigameLevel = MinigameBalloonData.Num() - 1;
		}
		//ǳ���� �������� ���� ����
		CorrectBalloonNumber = 1;
		//������ ���缭 ������ ���̺��� ���� ��������
		BalloonNum = MinigameBalloonData[CurrentMinigameLevel]->BalloonNum;
		BalloonSpeed = MinigameBalloonData[CurrentMinigameLevel]->BalloonSpeed;
		LineRandomInterval = MinigameBalloonData[CurrentMinigameLevel]->LineRandomInterval;
		break;
	}

	//ǳ�� ����ŭ ����
	int32 Count = 0;
	int32 MaxCount = BalloonNum;
	while (BalloonNum > 0)
	{
		//���� ������ġ ����
		//n��° ���� BalloonSpawnPoints�� ���� ����
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
			
			//������� ��Ͽ� �߰�
			UsingBalloons.Add(Balloon);
			//ǳ���� ���� ����
			int ZSize = Balloon->GetActorMesh()->GetBounds().GetBox().GetSize().Z;
			//�ӵ��� ǳ�� ���̸�ŭ ������ ���� �ð� 
			float WaitTime = (ZSize/2) / BalloonSpeed;
			float RandomInterVal = FMath::RandRange(0.0f, LineRandomInterval);
			//�����ؾ��Ѵ�. ���� ���� �ð��� �����ͼ� ���� �����ð��� �����ش�.
			float CurrentBalloonSpawnTime = BalloonSpawner->PreviousSpawnTime+ WaitTime + RandomInterVal;
			BalloonSpawner->PreviousSpawnTime = CurrentBalloonSpawnTime;
	
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
	Balloon->RemoveFromRoot();
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
		//������ ���ڶ�� ���͸� ����
		if (CorrectBalloonNumber == MinigameBalloonData[CurrentMinigameLevel]->BalloonNum)
		{
			//�÷��̾� ã��
			APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			if (AHPPawn* Player = Cast<AHPPawn>(PlayerPawn))
			{
				Player->ChargeBattery(CurrentMinigameLevel);
			}

			//ǳ�� ��Ȱ��ȭ �� Ǯ�� �ֱ�
			Balloon->AddToRoot();
			Balloon->DeactivateToSave();
			BalloonQueue.Enqueue(Balloon);

			//ResetWeapon();
		}
		//�˸��� ���ڸ� ����ٸ� �н�
		else if (CorrectBalloonNumber == Balloon->GetNumberInWidget())
		{
			++CorrectBalloonNumber;

			//ǳ�� ��Ȱ��ȭ �� Ǯ�� �ֱ�
			Balloon->AddToRoot();
			Balloon->DeactivateToSave();
			BalloonQueue.Enqueue(Balloon);
			return;
		}
		
		//Ʋ�� ���� or ������ ǳ���� ����ٸ� �������� 
		StopMinigame();
		//�ٽ� ���� ����
		StartMinigame();
		break;
	}
}

void AMinigameManager::SetMinigame()
{
	//�������� ���� �ߴ�
	StopMinigame();
	if (IHPMinigameDataInterface* gs = Cast<IHPMinigameDataInterface>(GetWorld()->GetGameState()))
	{
		//�̴ϰ��� �ٲٱ�
		CurrentMinigame = (EMinigame)gs->GetCurrentDay();
	}
	
}

