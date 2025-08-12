// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BalloonSpawnPoint.generated.h"

UCLASS()
class HORRORPROJECT_API ABalloonSpawnPoint : public AActor
{
	GENERATED_BODY()
	friend class AMinigameManager;
public:	
	// Sets default values for this actor's properties
	ABalloonSpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	int32 ScreenBalloonNumber = 0;;
	float PreviousSpawnTime = 0.0f;;
};
