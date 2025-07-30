// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Alarmbutton.generated.h"

//알람 버튼이 눌렸다는 것을 알리는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnAlarmbuttonDelegate);

UCLASS()
class HORRORPROJECT_API AAlarmbutton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAlarmbutton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	//블루프린트에서 델리게이트를 Broadcast 하기 위한 임시 함수
	UFUNCTION(BlueprintCallable)
	void OnAlarmbutton();
public:
	FOnAlarmbuttonDelegate OnAlarmbuttonDelegate;

};
