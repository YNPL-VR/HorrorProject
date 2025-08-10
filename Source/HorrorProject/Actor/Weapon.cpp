// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CatchWeaponDynamicMultiDelegate.Clear();
	PutWeaponDynamicMultiDelegate.Clear();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::CatchWeapon()
{
	if (CatchWeaponDynamicMultiDelegate.IsBound())
	{
		CatchWeaponDynamicMultiDelegate.Broadcast();
	}
	
}

void AWeapon::PutWeapon()
{
	if (PutWeaponDynamicMultiDelegate.IsBound())
	{
		PutWeaponDynamicMultiDelegate.Broadcast();
	}
}

