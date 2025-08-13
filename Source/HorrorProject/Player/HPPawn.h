// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "HPPawn.generated.h"

//알람 버튼이 눌렸다는 것을 알리는 델리게이트
DECLARE_MULTICAST_DELEGATE(FSuccessConsumeBatteryDelegate);
DECLARE_MULTICAST_DELEGATE(FFailedConsumeBatteryDelegate);
DECLARE_DELEGATE_OneParam(FUIConsumeBatteryDelegate, float /* CurrentBattery */);


UCLASS()
class HORRORPROJECT_API AHPPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHPPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<class UInputMappingContext> DefaultMappingContext;
	//#if !(UE_BUILD_SHIPPING)
	//캐릭터 카메라 움직임 추가하기
public:
	//배터리 소모 함수
	UFUNCTION()
	void ConsumeBattery();
	//배터리 충전 함수
	void ChargeBattery(int32 MinigameLevel);
	FORCEINLINE float GetBattery() const { return CurrentBattery; }
	FORCEINLINE void SetBattery(const float InBattery) {  CurrentBattery = InBattery; }
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
public:
	FSuccessConsumeBatteryDelegate SuccessConsumeBatteryDelegate;
	FUIConsumeBatteryDelegate UIConsumeBatteryDelegate;
	FFailedConsumeBatteryDelegate FailedConsumeBatteryDelegate;
private:
	// Todo : 자유 시점을 위해 시도 했지만 실패
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	float CurrentBattery;

	// HUD - 배터리량 표시
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget")
	class UWidgetComponent* HUDWidgetComponent;*/

};
