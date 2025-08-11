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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget")
	class UWidgetComponent* NumberWidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* BalloonMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UMaterialInstanceDynamic* DynamicMaterialInstance;
	// Todo : �� ���� �����ϰ� , Spawn�� �ʰ� �ؼ� �� �� �ֱ�
};
