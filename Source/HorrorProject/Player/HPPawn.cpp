// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HPPawn.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
AHPPawn::AHPPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Default_Finder(TEXT("/Game/Input/IMC_HP.IMC_HP"));
	if (IMC_Default_Finder.Succeeded())
	{
		DefaultMappingContext = IMC_Default_Finder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputMoveFinder(TEXT("/Game/Input/IA_HPMove.IA_HPMove"));
	if (InputMoveFinder.Succeeded())
	{
		MoveAction = InputMoveFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputLookFinder(TEXT("/Game/Input/IA_HPLook.IA_HPLook"));
	if (InputLookFinder.Succeeded())
	{
		LookAction = InputLookFinder.Object;
	}
}

// Called when the game starts or when spawned
void AHPPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (!DefaultMappingContext.IsNull())
			{
				UInputMappingContext* MappingContext = DefaultMappingContext.LoadSynchronous();
				Subsystem->AddMappingContext(MappingContext, 0 /*Priority*/);
			}
		}
	}

}

// Called every frame
void AHPPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void AHPPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* Subsystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Subsystem->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AHPPawn::Move);
		//Subsystem->BindAction(LookAction,ETriggerEvent::Triggered,this,&AHPPawn::Look);
	}
}

void AHPPawn::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	FVector CurrentLocation = GetActorLocation();
	CurrentLocation.X += MovementVector.X;
	CurrentLocation.Y += MovementVector.Y;
	SetActorLocation(CurrentLocation);
	//if (nullptr!=Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, MovementVector.X * 500.0f);
		AddMovementInput(RightDirection, MovementVector.Y * 500.0f);
	}

}

void AHPPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		//Todo: vr모드일때 디버그 모드일때 bUseControllerRotationYaw가 true
		AddControllerYawInput(LookAxisVector.X * 1.5f);
		AddControllerPitchInput(LookAxisVector.Y * 1.5f);
	}
}

