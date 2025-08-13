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
	//현재 미니게임 종류
	EMinigame CurrentMinigame = EMinigame::NumBalloon;
	//현재 미니게임 난이도
	int32 CurrentMinigameLevel = 0;

	//무기를 스폰해 놓고 비활성화 시킨 후 해당 미니게임마다 쓰고있는 무기는 비활성화하고 해당 무기를 활성화
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AWeapon> CurrentWeapon;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class AWeapon>> AllWeapons;
	UPROPERTY()
	TSubclassOf<class AWeapon> GunClass;

	//Player가 총을 SpawnPoint에서 집은 후 놓았을 때,Spawn의 카운팅이 시작된다 - 다시 잡았을때 Spawn카운팅을 초기화시켜야할것같으므로 잡을때와 놓았을때의 델리게이트 -> 타이머
	//무기 스폰 타임
	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"),Category = "Spawn")
	float SpawnTime;
	FTimerHandle SpawnWeaponHandle;

	//풍선 스폰 위치
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<class ABalloonSpawnPoint*> BalloonSpawnPoints;
	//풍선 오브젝트 풀
	//풍선 스폰 - 타이머 필요 , 타이머를 풍선과 짝지어줌
	TQueue<class ABalloon*> BalloonQueue;
	//사용중인 풍선
	TArray<class ABalloon*> UsingBalloons;

	//미니게임 데이터 테이블 데이터
	TArray<FNumBalloon*> MinigameBalloonData;
	UPROPERTY()
	UDataTable* MinigameBalloonDataTable;

	//Todo : 잘못된 풍선을 격추시켰을 경우 - DeactivateAllBalloon()
	//마지막 풍선을 격추시켰을 경우 - CurrentMinigameLevel 에 맞추어 배터리 충전

	//Todo : 어떻게 옳은 풍선을 구분할 것인가? 순서를 저장해둠
	//번호는 번호, 색깔은 색깔 enum , 나머지는 검사없이 건너뛰기
	//EMinigame::NumBalloon 일경우 숫자 ++, 
	int32 CorrectBalloonNumber;
};
