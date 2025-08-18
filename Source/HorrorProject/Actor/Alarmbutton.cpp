// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Alarmbutton.h"
#include "Player/HPPawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAlarmbutton::AAlarmbutton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAlarmbutton::BeginPlay()
{
	Super::BeginPlay();
	
	//알람 성공/실패 델리게이트 등록
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (AHPPawn* Player = Cast<AHPPawn>(PlayerPawn))
	{
		Player->FailedConsumeBatteryDelegate.AddUFunction(this, FName("FailedButtonResult"));
		Player->SuccessConsumeBatteryDelegate.AddUFunction(this, FName("SuccessButtonResult"));
	}
}

void AAlarmbutton::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnAlarmbuttonDelegate.Clear();
}

// Called every frame
void AAlarmbutton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAlarmbutton::OnAlarmbutton()
{
	if (OnAlarmbuttonDelegate.IsBound())
		OnAlarmbuttonDelegate.Broadcast();
}

void AAlarmbutton::FailedButtonResult()
{
	FailedButtonResultBP();
}

void AAlarmbutton::SuccessButtonResult()
{
	SuccessButtonResultBP();
}

