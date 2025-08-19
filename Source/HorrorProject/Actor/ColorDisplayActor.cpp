// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/ColorDisplayActor.h"

// Sets default values
AColorDisplayActor::AColorDisplayActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//색을 보여줄 컴포넌트
	DisplayMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMeshComponent"));
	DisplayMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	SetRootComponent(DisplayMeshComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_ChamferCube.SM_ChamferCube'"));
	if (MeshFinder.Succeeded())
	{
		DisplayMeshComponent->SetStaticMesh(MeshFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MaterialFinder(TEXT("/Game/LSJ/Material/MI_Balloon.MI_Balloon"));
	if (MaterialFinder.Succeeded())
	{
		DisplayMeshComponent->SetMaterial(0, MaterialFinder.Object);
	}

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

// Called when the game starts or when spawned
void AColorDisplayActor::BeginPlay()
{
	Super::BeginPlay();
	
	//다이나믹 매터리얼 설정
	if (DisplayMeshComponent && DisplayMeshComponent->GetMaterial(0))
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(DisplayMeshComponent->GetMaterial(0), this);
		FName ParameterName = FName("Param");
		DynamicMaterialInstance->SetVectorParameterValue(ParameterName, FVector(1,1,1));
		DisplayMeshComponent->SetMaterial(0, DynamicMaterialInstance);
	}

	FTimerHandle t;
	FTimerDelegate td;
	/*td.BindUFunction(this, FName("SetDisplayColor"), FVector(0, 0, 0));
	GetWorld()->GetTimerManager().SetTimer(t, td, 1.0f,false);*/
	td.BindUFunction(this, FName("SetDisplayColor"), FVector(1.f, 0.f, 0.f));
	GetWorld()->GetTimerManager().SetTimer(t, td, 2.0f, false);
}

// Called every frame
void AColorDisplayActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AColorDisplayActor::SetDisplayColor(FVector Color)
{
	FName ParameterName = FName("Param");
	DynamicMaterialInstance->SetVectorParameterValue(ParameterName, Color);
}

