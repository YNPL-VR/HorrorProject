// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

private:
	//현재 미니게임 종류
	EMinigame CurrentMinigame = EMinigame::NumBalloon;
	//무기를 스폰해 놓고 비활성화 시킨 후 해당 미니게임마다 쓰고있는 무기는 비활성화하고 해당 무기를 활성화하자
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

};
