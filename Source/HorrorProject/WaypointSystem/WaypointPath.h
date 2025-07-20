// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaypointPath.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FWaypointInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MinWaitSecond;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxWaitSecond;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Speed;
};




UCLASS()
class HORRORPROJECT_API AWaypointPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaypointPath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetCurrentWaypoint(int32 InWaypoint) { currentWaypoint = InWaypoint; }
	FORCEINLINE int GetCurrentWaypoint() const { return currentWaypoint; }



public:
	// �����Ϳ��� ������Ƽ�� ����Ǿ��� �� ȣ��� �Լ��Դϴ�.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	//��������Ʈ�� ������������ �Լ���
	UFUNCTION()
	void ArrivedOnWaypoint();
	// WaypointInfo �� ������Ʈ�ϴ� �Լ��Դϴ�.
	void UpdateWaypointInfo();
	//���� ��������Ʈ�� �̵��ϴ� �Լ�
	void MoveToNextWaypoint();

private:
	UPROPERTY(EditAnywhere, Category = Path, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AHGCharacterEnemy> TargetClass;

	UPROPERTY()
	TObjectPtr<class AHGCharacterEnemy> Target;

	UPROPERTY(EditAnywhere, Category=Path, meta = (AllowPrivateAccess = "true", DisplayName = "BP_Waypoint"))
	TArray<TObjectPtr<class AWaypoint>> Path;

	UPROPERTY(VisibleAnywhere, Category=Path)
	TArray<FWaypointInfo> WaypointInfo;

	UPROPERTY(VisibleInstanceOnly, Category = Path)
	int32 currentWaypoint;

	FTimerHandle PathHandle;
};
