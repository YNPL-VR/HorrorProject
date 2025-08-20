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
	//�� ������ ���� ���͸���
	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* DynamicMaterialInstance;
	//���� �����ֱ� ���� Ÿ�̸�
	FTimerHandle ShowColorTimer;
	//�� ���ӿ� ������ ��������
	TArray<TPair<float, FLinearColor>> TimeColor;
};
