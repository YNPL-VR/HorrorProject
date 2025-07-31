// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Alarmbutton.generated.h"

//알람 버튼이 눌렸다는 것을 알리는 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnAlarmbuttonDelegate);

/*
	https://www.youtube.com/watch?v=AHZcFyHLGqQ 영상 참고
	physics constraints 컴포넌트를 사용하여 버튼의 눌림을 구현
	관련 설정 내용
	{
		//Todo : 버튼을 제작할때 메시가 움직일 Component,기준이 될 Component 2개로 나누어져야한다.
		1. physics constraints
			- Component Name 1 : 움직일 Component
			- Component Name 2 : 기준이 될 Component
			- Linear Limits-> Z Motion : Limited, Limit 움직이게 만들 길이
			- Linear Motor -> Position Target -> Z 움직이게 할 축 체크, Strength : 1500 숫자가 클수록 원래 위치로 돌아가는 힘이 강함
		2. 기준이 될 Component
			- Collision -> Object Type = WorldStatic 이어야 한다.
		3. 움직일 Component
			- Collision -> Physics Only 로 물리 충돌 가능하게 만들었다
		4. 상호작용할 Component
			- Collision -> Physics Only 로 물리 충돌 가능하게 만들었다 
	}
*/
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
