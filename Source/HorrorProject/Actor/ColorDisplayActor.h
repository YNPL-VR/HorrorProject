// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ColorDisplayActor.generated.h"

UCLASS()
class HORRORPROJECT_API AColorDisplayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AColorDisplayActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void SetDisplayColor(int32 Index);

	void ClearShowColorList();
	void AddShowColor(float InRate, FLinearColor DisplayColor);
	void StartShowColor();
	FLinearColor GetBalloonColor(int32 Index);
	FORCEINLINE int32 GetBalloonColorLastIdx() { return TimeColor.Num()-1; }
private:
	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* DisplayMeshComponent;
	//색 변경을 위한 매터리얼
	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* DynamicMaterialInstance;
	//색을 보여주기 위한 타이머
	FTimerHandle ShowColorTimer;
	//한 게임에 보여줄 색상정보
	TArray<TPair<float, FLinearColor>> TimeColor;
};
