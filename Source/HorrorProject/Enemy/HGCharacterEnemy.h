// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HGCharacterEnemy.generated.h"

UENUM(BlueprintType)
enum class EStateEnemy : uint8
{
	Waiting = 0,
	Running,
	Arrival
};

UCLASS()
class HORRORPROJECT_API AHGCharacterEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHGCharacterEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetState(EStateEnemy InState);
	FORCEINLINE EStateEnemy GetState() { return CurrentState; }

	void SetWalkSpeed(float InSpeed);
private:
	EStateEnemy CurrentState;
};
