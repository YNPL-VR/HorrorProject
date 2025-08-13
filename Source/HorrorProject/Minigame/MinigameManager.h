// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "Datatable/NumBalloon.h"
#include "Containers/Queue.h"
#include "MinigameManager.generated.h"


UENUM(BlueprintType)
enum class EMinigame : uint8
{
	NumBalloon = 0,
	ColorBalloon,
	DartBalloon,
	BatBall
};

UCLASS()
class HORRORPROJECT_API AMinigameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinigameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void SetMinigame(EMinigame minigame) { CurrentMinigame = minigame; }
	UFUNCTION()
	void OnSpawnWeaponTimer();
	UFUNCTION()
	void OffSpawnWeaponTimer();
	void ResetWeapon();

	UFUNCTION()
	void StartMinigame();
	void StopMinigame();
	UFUNCTION()
	void SpawnBalloon(FVector Location, FRotator Rotation, float Speed,int32 Number, ABalloon*   Balloon);
	void DeactivateAllBalloon();
	UFUNCTION()
	void CheckCorrectBalloon(class ABalloon* Balloon);
private:
	//���� �̴ϰ��� ����
	EMinigame CurrentMinigame = EMinigame::NumBalloon;
	//���� �̴ϰ��� ���̵�
	int32 CurrentMinigameLevel = 0;

	//���⸦ ������ ���� ��Ȱ��ȭ ��Ų �� �ش� �̴ϰ��Ӹ��� �����ִ� ����� ��Ȱ��ȭ�ϰ� �ش� ���⸦ Ȱ��ȭ
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AWeapon> CurrentWeapon;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class AWeapon>> AllWeapons;
	UPROPERTY()
	TSubclassOf<class AWeapon> GunClass;

	//Player�� ���� SpawnPoint���� ���� �� ������ ��,Spawn�� ī������ ���۵ȴ� - �ٽ� ������� Spawnī������ �ʱ�ȭ���Ѿ��ҰͰ����Ƿ� �������� ���������� ��������Ʈ -> Ÿ�̸�
	//���� ���� Ÿ��
	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"),Category = "Spawn")
	float SpawnTime;
	FTimerHandle SpawnWeaponHandle;

	//ǳ�� ���� ��ġ
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<class ABalloonSpawnPoint*> BalloonSpawnPoints;
	//ǳ�� ������Ʈ Ǯ
	//ǳ�� ���� - Ÿ�̸� �ʿ� , Ÿ�̸Ӹ� ǳ���� ¦������
	TQueue<class ABalloon*> BalloonQueue;
	//������� ǳ��
	TArray<class ABalloon*> UsingBalloons;

	//�̴ϰ��� ������ ���̺� ������
	TArray<FNumBalloon*> MinigameBalloonData;
	UPROPERTY()
	UDataTable* MinigameBalloonDataTable;

	//Todo : �߸��� ǳ���� ���߽����� ��� - DeactivateAllBalloon()
	//������ ǳ���� ���߽����� ��� - CurrentMinigameLevel �� ���߾� ���͸� ����

	//Todo : ��� ���� ǳ���� ������ ���ΰ�? ������ �����ص�
	//��ȣ�� ��ȣ, ������ ���� enum , �������� �˻���� �ǳʶٱ�
	//EMinigame::NumBalloon �ϰ�� ���� ++, 
	int32 CorrectBalloonNumber;
};
