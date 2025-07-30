// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Alarmbutton.h"

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

