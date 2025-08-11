// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Balloon.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABalloon::ABalloon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NumberWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NumberWidgetComponent"));
	NumberWidgetComponent->SetCollisionProfileName(FName("NoCollision"));

	BalloonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BalloonMeshComponent"));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (SphereMeshFinder.Succeeded())
	{
		BalloonMeshComponent->SetStaticMesh(SphereMeshFinder.Object);
		BalloonMeshComponent->SetSimulatePhysics(true);
		//BalloonMeshComponent->SetEnableGravity()
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> BalloonMaterialFinder(TEXT("/Game/LSJ/Material/MI_Balloon.MI_Balloon"));
	if (BalloonMaterialFinder.Succeeded())
	{
		BalloonMeshComponent->SetMaterial(0, BalloonMaterialFinder.Object);
	}

	/*static ConstructorHelpers::FClassFinder<UUserWidget> HUD(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/LSJ/UI/WBP_HPHUD.WBP_HPHUD_C'"));
	if (HUD.Succeeded())
	{
		NumberWidgetComponent->SetWidgetClass(HUD.Class);
		NumberWidgetComponent->SetDrawSize(FVector2D(1920, 1080));
	}

	*/
}

// Called when the game starts or when spawned
void ABalloon::BeginPlay()
{
	Super::BeginPlay();
	if (BalloonMeshComponent && BalloonMeshComponent->GetMaterial(0))
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(BalloonMeshComponent->GetMaterial(0), this);
		FName ParameterName = FName("Param");
		DynamicMaterialInstance->SetVectorParameterValue(ParameterName, Color);
		BalloonMeshComponent->SetMaterial(0, DynamicMaterialInstance);
	}
}

// Called every frame
void ABalloon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABalloon::SetColor(FVector InColor)
{
	Color = InColor;
}

void ABalloon::ActivateToUse()
{
	SetActorHiddenInGame(false);
	//SetActorEnableCollision(ECollision)
	SetActorTickEnabled(true);
}

void ABalloon::DeactivateToSave()
{
	SetActorHiddenInGame(true);
	//SetActorEnableCollision(ECollision)
	SetActorTickEnabled(false);
}

