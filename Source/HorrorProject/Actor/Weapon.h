// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCatchWeaponDynamicMultiDelegate, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPutWeaponDynamicMultiDelegate);

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Gun,
	Dart,
	Bat
};

UCLASS()
class HORRORPROJECT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void CatchWeapon();
	UFUNCTION(BlueprintCallable)
	void PutWeapon();

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Weapon")
	FCatchWeaponDynamicMultiDelegate CatchWeaponDynamicMultiDelegate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Weapon")
	FPutWeaponDynamicMultiDelegate PutWeaponDynamicMultiDelegate;
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	FTimerHandle SpawnWeaponHandle;
	UPROPERTY(BlueprintReadWrite,Category = "Weapon")
	EWeaponType WeaponType;
};
