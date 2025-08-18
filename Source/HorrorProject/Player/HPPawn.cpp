// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HPPawn.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Game/HPGameStateBase.h"
#include "WaypointSystem/AWaypointManager.h"
#include "EngineUtils.h"
#include "Actor/Alarmbutton.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"

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

	/*HUDWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HUDWidgetComponent"));
	HUDWidgetComponent->SetCollisionProfileName(FName("NoCollision"));
	
	static ConstructorHelpers::FClassFinder<UUserWidget> HUD(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/LSJ/UI/WBP_HPHUD.WBP_HPHUD_C'"));
	if (HUD.Succeeded())
	{
		HUDWidgetComponent->SetWidgetClass(HUD.Class);
		HUDWidgetComponent->SetDrawSize(FVector2D(1920, 1080));
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/Game/LSJ/Material/M_HPHUD.M_HPHUD"));
	if (MaterialFinder.Succeeded())
	{
		HUDWidgetComponent->SetMaterial(0,MaterialFinder.Object);
	}*/
	
	
}

// Called when the game starts or when spawned
void AHPPawn::BeginPlay()
{
	Super::BeginPlay();

	CurrentBattery = 100.f;
	
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

	// Todo : 버튼 액터에서 하기 2
	/*UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<AAWaypointManager> It(World); It; ++It)
		{
			AWaypointManager = *It;

			break;
		}
	}*/
	
	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<AAlarmbutton> It(World); It; ++It)
		{
			AAlarmbutton* Alarmbutton = *It;
			if (Alarmbutton)
			{
				Alarmbutton->OnAlarmbuttonDelegate.AddUFunction(this, FName("ConsumeBattery"));
			}

			break;
		}
	}


}

void AHPPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	SuccessConsumeBatteryDelegate.Clear();
	FailedConsumeBatteryDelegate.Clear();
	UIConsumeBatteryDelegate.Unbind();
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

void AHPPawn::ConsumeBattery()
{
	IHPMinigameDataInterface* gs = GetWorld()->GetGameState< IHPMinigameDataInterface>();
	if (gs)
	{
		float ConsumeBattery = gs->GetConsumeAlarmBattery(gs->GetMinigameLevel());

		if (CurrentBattery >= ConsumeBattery)
		{
			//CurrentBattery 를 현재 레벨 난이도에 해당하는 소모배터리량 만큼 감소
			CurrentBattery -= ConsumeBattery;
			// Todo : UI에서도 감소
			if (SuccessConsumeBatteryDelegate.IsBound())
			{
				SuccessConsumeBatteryDelegate.Broadcast();
			}
			if (UIConsumeBatteryDelegate.IsBound())
			{
				UIConsumeBatteryDelegate.Execute(CurrentBattery);
			}
			
		}
		else
		{

			if (FailedConsumeBatteryDelegate.IsBound())
			{
				FailedConsumeBatteryDelegate.Broadcast();
			}
		}
		
	}
}

void AHPPawn::ChargeBattery()
{
	IHPMinigameDataInterface* gs = GetWorld()->GetGameState< IHPMinigameDataInterface>();
	if (gs)
	{
		//CurrentBattery 를 현재 레벨 난이도에 해당하는 충전배터리량 만큼 증가
		//CurrentBattery 100 이상으로 높아지지 않게 처리
		CurrentBattery = FMath::Max<float>(100.f, CurrentBattery + gs->GetChargeBattery(gs->GetMinigameLevel()));
		// Todo : UI에서도 증가
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
		//Todo: 자유모드를 위해서 vr모드일때 디버그 모드일때 bUseControllerRotationYaw가 true
		AddControllerYawInput(LookAxisVector.X * 1.5f);
		AddControllerPitchInput(LookAxisVector.Y * 1.5f);
	}
}

