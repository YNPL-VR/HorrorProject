// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Waypoint.generated.h"

/**
 * 
 */
UCLASS()
class HORRORPROJECT_API AWaypoint : public AActor
{
	GENERATED_BODY()
public:
	AWaypoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE float GetMinWaitSecond() const { return MinWaitSecond; }
	FORCEINLINE float GetMaxWaitSecond() const { return MaxWaitSecond; }
	FORCEINLINE float GetSpeed() const { return Speed; }

private:
	UPROPERTY(EditAnywhere, Category = Path, Meta = (AllowPrivateAccess = "true"))
	float MinWaitSecond;
	UPROPERTY(EditAnywhere, Category = Path, Meta = (AllowPrivateAccess = "true"))
	float MaxWaitSecond;
	UPROPERTY(EditAnywhere, Category = Path, Meta = (AllowPrivateAccess = "true"))
	float Speed;
};
