// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Balloon.generated.h"

DECLARE_DELEGATE_OneParam(FHitBalloonWithWeapon, ABalloon*)

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetColor(FVector InColor);

	void ActivateToUse(FVector Location, FRotator Rotation, float Speed);
	void DeactivateToSave();
	class UStaticMesh* GetActorMesh() const;
	void SetNumberInWidget(int32 Num);
	inline int32 GetNumberInWidget() { return ScreenBalloonNumber; }
	void SetNumberWidgetVisible(bool bVisible);

	//���� Ÿ�̸� �ڵ�
	FTimerHandle SpawnTimerHandle;

	//���ݿ� �¾����� ��������Ʈ
	FHitBalloonWithWeapon HitBalloonWithWeapon;
protected:
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class USphereComponent* SphereComponent;
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
