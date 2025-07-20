// Fill out your copyright notice in the Description page of Project Settings.


#include "HGCharacterEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HGEnemyAIController.h"


// Sets default values
AHGCharacterEnemy::AHGCharacterEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	//평면 설정
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//Movement
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 200.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.f), FRotator(0.0f, -90.0f, 0.0f));

	CurrentState = EStateEnemy::Waiting;

	AIControllerClass = AHGEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AHGCharacterEnemy::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void AHGCharacterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHGCharacterEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHGCharacterEnemy::SetState(EStateEnemy InState)
{
	CurrentState = InState;
}

void AHGCharacterEnemy::SetWalkSpeed(float InSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = InSpeed;
}

