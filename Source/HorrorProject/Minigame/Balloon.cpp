// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Balloon.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UI/BalloonWidget.h"

// Sets default values
ABalloon::ABalloon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BalloonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BalloonMeshComponent"));
	BalloonMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	BalloonMeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	BalloonMeshComponent->SetLinearDamping(5.0f);
	BalloonMeshComponent->SetAngularDamping(5.0f);
	BalloonMeshComponent->SetMassOverrideInKg(NAME_None, 2.0f, true);
	BalloonMeshComponent->SetNotifyRigidBodyCollision(true);
	SetRootComponent(BalloonMeshComponent);

	NumberWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NumberWidgetComponent"));
	NumberWidgetComponent->SetCollisionProfileName(FName("NoCollision"));
	NumberWidgetComponent->SetVisibility(false);
	NumberWidgetComponent->SetupAttachment(RootComponent);
	NumberWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UBalloonWidget> BalloonWidgetFinder(TEXT("/Game/LSJ/UI/WBP_BalloonNumber.WBP_BalloonNumber_C"));
	if (BalloonWidgetFinder.Succeeded())
	{
		NumberWidgetComponent->SetWidgetClass(BalloonWidgetFinder.Class);
		NumberWidgetComponent->SetDrawSize(FVector2D(1920, 1080));
	}

	
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (SphereMeshFinder.Succeeded())
	{
		BalloonMeshComponent->SetStaticMesh(SphereMeshFinder.Object);
		
		//BalloonMeshComponent->SetEnableGravity()
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> BalloonMaterialFinder(TEXT("/Game/LSJ/Material/MI_Balloon.MI_Balloon"));
	if (BalloonMaterialFinder.Succeeded())
	{
		BalloonMeshComponent->SetMaterial(0, BalloonMaterialFinder.Object);
	}

	
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
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
		BalloonMeshComponent->OnComponentHit.AddDynamic(this, &ABalloon::OnComponentHit);
	}

	
}



void ABalloon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HitBalloonWithWeapon.Unbind();

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ABalloon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Áß·Â »ó¼â Èû
	const float GravityForce = BalloonMeshComponent->GetMass() * 980.0f;
	//ThrustForce
	const FVector TotalForce = FVector(0.0f, 0.0f, GravityForce + ThrustForce);
	BalloonMeshComponent->AddForce(TotalForce);
}

void ABalloon::SetColor(FVector InColor)
{
	Color = InColor;
}

void ABalloon::ActivateToUse(FVector Location, FRotator Rotation, float Speed)
{
	ThrustForce = Speed;
	SetActorLocation(Location);
	SetActorRotation(Rotation);
	SetActorHiddenInGame(false);
	BalloonMeshComponent->SetCollisionProfileName(FName("Balloon"));
	SetActorTickEnabled(true);
	BalloonMeshComponent->SetSimulatePhysics(true);
	BalloonMeshComponent->SetEnableGravity(true);
	UE_LOG(LogTemp, Warning, TEXT("z : %f"), Location.Z);
	FTimerHandle t;
	GetWorld()->GetTimerManager().SetTimer(t, [this]()
		{
			UE_LOG(LogTemp, Warning, TEXT("after z : %f"), GetActorLocation().Z);
		}, 1.0f, false);
}

void ABalloon::DeactivateToSave()
{
	SetActorHiddenInGame(true);
	//SetActorEnableCollision(ECollision)
	SetActorTickEnabled(false);
	BalloonMeshComponent->SetSimulatePhysics(false);
	BalloonMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	BalloonMeshComponent->SetEnableGravity(false);
}

UStaticMesh* ABalloon::GetActorMesh() const
{
	return BalloonMeshComponent->GetStaticMesh().Get();
}

void ABalloon::SetNumberInWidget(int32 Num)
{
	if (UBalloonWidget* widget = Cast<UBalloonWidget>(NumberWidgetComponent->GetWidget()))
	{
		ScreenBalloonNumber = Num;
		widget->SetTextNumber(Num);
	}
}

void ABalloon::SetNumberWidgetVisible(bool bVisible)
{
	NumberWidgetComponent->SetVisibility(bVisible);
}

void ABalloon::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->ActorHasTag(FName("Weapon")) && HitBalloonWithWeapon.IsBound())
	{
		HitBalloonWithWeapon.Execute(this);
	}
}

