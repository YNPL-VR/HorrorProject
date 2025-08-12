// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Balloon.generated.h"

UCLASS()
class HORRORPROJECT_API ABalloon : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABalloon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetColor(FVector InColor);

	void ActivateToUse(FVector Location, FRotator Rotation, float Speed);
	void DeactivateToSave();
	class UStaticMesh* GetActorMesh() const;
	void SetNumberInWidget(int32 Num);
	void SetNumberWidgetVisible(bool bVisible);
private:
	UPROPERTY(EditAnywhere, Category="Widget", meta = (AllowPrivateAccess="true"))
	class UWidgetComponent* NumberWidgetComponent;
	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BalloonMeshComponent;
	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* DynamicMaterialInstance;
	// Todo : �� ���� �����ϰ� , Spawn�� �ʰ� �ؼ� �� �� �ֱ�
	FVector Color;
	int32 ScreenBalloonNumber;
	//�ӵ���
	float ThrustForce = 0.0f;
};
