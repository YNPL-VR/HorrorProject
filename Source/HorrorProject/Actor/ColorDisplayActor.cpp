// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/ColorDisplayActor.h"

// Sets default values
AColorDisplayActor::AColorDisplayActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//���� ������ ������Ʈ
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
	
	//���̳��� ���͸��� ����
	if (DisplayMeshComponent && DisplayMeshComponent->GetMaterial(0))
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(DisplayMeshComponent->GetMaterial(0), this);
		FName ParameterName = FName("Param");
		DynamicMaterialInstance->SetVectorParameterValue(ParameterName, FVector(1,1,1));
		DisplayMeshComponent->SetMaterial(0, DynamicMaterialInstance);
	}
}

// Called every frame
void AColorDisplayActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AColorDisplayActor::SetDisplayColor(int32 Index)
{
	GetWorld()->GetTimerManager().ClearTimer(ShowColorTimer);

	FName ParameterName = FName("Param");
	//��������� ������ȿ��
	DynamicMaterialInstance->SetVectorParameterValue(ParameterName, TimeColor[Index].Value);

	

	//������ �ε������ Ÿ�̸� �۵� ����
	if (TimeColor.Num() - 1 <= Index)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(ShowColorTimer,
		[this,Index]()
		{
			SetDisplayColor(Index+1);
		}, TimeColor[Index+1].Key, false);
}

void AColorDisplayActor::ClearShowColorList()
{
	TimeColor.Empty();
}

void AColorDisplayActor::AddShowColor(float InRate, FLinearColor DisplayColor)
{
	TimeColor.Add({ InRate,DisplayColor });
}

void AColorDisplayActor::StartShowColor()
{
	if (TimeColor.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("AColorDisplayActor::StartShowColor - NO Color Time"));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(ShowColorTimer,
		[this]()
		{
			SetDisplayColor(0);
		},TimeColor[0].Key+1.0f,false);
}

FLinearColor AColorDisplayActor::GetBalloonColor(int32 Index)
{
	if (!TimeColor.IsValidIndex(Index))
		return FLinearColor(1,1,1,1);
	return TimeColor[Index].Value; 
}

