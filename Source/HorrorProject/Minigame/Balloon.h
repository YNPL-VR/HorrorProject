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
	// Todo : 색 변수 선언하고 , Spawn을 늦게 해서 색 값 넣기
};
