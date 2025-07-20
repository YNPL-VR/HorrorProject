// Fill out your copyright notice in the Description page of Project Settings.


#include "WaypointSystem/Waypoint.h"

AWaypoint::AWaypoint()
{
	PrimaryActorTick.bCanEverTick = false;

	MinWaitSecond = 0.0f;
	MaxWaitSecond = 1.0f;
	Speed = 500.0f;
}

// Called when the game starts or when spawned
void AWaypoint::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWaypoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


