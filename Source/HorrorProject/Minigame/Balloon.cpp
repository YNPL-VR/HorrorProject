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
	NumberWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	NumberWidgetComponent->SetRelativeLocation(FVector(50.0f,0.f,0.f));
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

	//중력 상쇄 힘
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
	//Todo : 충돌이 풍선과 안되는 경우가 있다
	//아마 sphere 콜리전이 없어서 또는 속도가 너무 빨라서

	if (OtherActor->ActorHasTag(FName("Weapon")) && HitBalloonWithWeapon.IsBound())
	{

		HitBalloonWithWeapon.Execute(this);
		
	}
	else if (OtherActor->ActorHasTag(FName("Deadzone")) && HitBalloonWithWeapon.IsBound())
	{
		//실패를 알리기 위해
		ScreenBalloonNumber = -1;
		HitBalloonWithWeapon.Execute(this);
	}
}

