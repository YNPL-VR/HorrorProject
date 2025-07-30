// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "HPPawn.generated.h"

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<class UInputMappingContext> DefaultMappingContext;
	//#if !(UE_BUILD_SHIPPING)
	//ĳ���� ī�޶� ������ �߰��ϱ�
public:
	UFUNCTION()
	void ConsumeBattery();
	FORCEINLINE float GetBattery() const { return CurrentBattery; }
	FORCEINLINE void SetBattery(const float InBattery) {  CurrentBattery = InBattery; }
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
private:
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	// Todo : ��ư ���Ϳ��� �ϱ� 3
	/*UPROPERTY()
	TObjectPtr<class AAWaypointManager> AWaypointManager;*/

	float CurrentBattery;
};
