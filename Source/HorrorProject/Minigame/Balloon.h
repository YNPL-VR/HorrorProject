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

	void ActivateToUse();
	void DeactivateToSave();
private:
	UPROPERTY(BlueprintReadOnly, Category="Widget", meta = (AllowPrivateAccess="true"))
	class UWidgetComponent* NumberWidgetComponent;
	UPROPERTY(BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BalloonMeshComponent;
	UPROPERTY(BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* DynamicMaterialInstance;
	// Todo : �� ���� �����ϰ� , Spawn�� �ʰ� �ؼ� �� �� �ֱ�
	FVector Color;
};
