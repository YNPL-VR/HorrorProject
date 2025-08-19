// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Balloon.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "UI/BalloonWidget.h"

// Sets default values
ABalloon::ABalloon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//�ݸ��� ����
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
	SphereComponent->SetCollisionProfileName(FName("NoCollision"));
	SphereComponent->SetLinearDamping(5.0f);
	SphereComponent->SetAngularDamping(5.0f);
	SphereComponent->SetSphereRadius(25.0f);
	SphereComponent->SetMassOverrideInKg(NAME_None, 2.0f, true);
	SphereComponent->SetNotifyRigidBodyCollision(true);
	SetRootComponent(SphereComponent);

	//�޽� ����
	BalloonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BalloonMeshComponent"));
	BalloonMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	BalloonMeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	BalloonMeshComponent->SetupAttachment(RootComponent);

	//���� ���� ����
	NumberWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NumberWidgetComponent"));
	NumberWidgetComponent->SetCollisionProfileName(FName("NoCollision"));
	NumberWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NumberWidgetComponent->SetVisibility(false);
	NumberWidgetComponent->SetupAttachment(RootComponent);
	NumberWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	NumberWidgetComponent->SetRelativeLocation(FVector(25.0f,0.f,0.f));
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

	//���̳��� ���͸��� ����
	if (SphereComponent && BalloonMeshComponent->GetMaterial(0))
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(BalloonMeshComponent->GetMaterial(0), this);
		FName ParameterName = FName("Param");
		DynamicMaterialInstance->SetVectorParameterValue(ParameterName, Color);
		BalloonMeshComponent->SetMaterial(0, DynamicMaterialInstance);
		//�浹 �̺�Ʈ �Լ����
		SphereComponent->OnComponentHit.AddDynamic(this, &ABalloon::OnComponentHit);
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

	//�߷� ��� ��
	const float GravityForce = SphereComponent->GetMass() * 980.0f;
	//���� �ö󰡴� ��
	const FVector TotalForce = FVector(0.0f, 0.0f, GravityForce + ThrustForce);
	SphereComponent->AddForce(TotalForce);
}

void ABalloon::SetColor(FVector InColor)
{
	Color = InColor;
}
//Ȱ��ȭ ����
void ABalloon::ActivateToUse(FVector Location, FRotator Rotation, float Speed)
{
	ThrustForce = Speed;
	SetActorLocation(Location);
	SetActorRotation(Rotation);
	SetActorHiddenInGame(false);
	SphereComponent->SetCollisionProfileName(FName("Balloon"));
	SetActorTickEnabled(true);
	SphereComponent->SetSimulatePhysics(true);
	SphereComponent->SetEnableGravity(true);

	//Todo : ǳ�� �׽�Ʈ �����
	UE_LOG(LogTemp, Warning, TEXT("z : %f"), Location.Z);
	FTimerHandle t;
	GetWorld()->GetTimerManager().SetTimer(t, [this]()
		{
			UE_LOG(LogTemp, Warning, TEXT("after z : %f"), GetActorLocation().Z);
		}, 1.0f, false);
}
//��Ȱ��ȭ ����
void ABalloon::DeactivateToSave()
{
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SphereComponent->SetSimulatePhysics(false);
	SphereComponent->SetCollisionProfileName(FName("NoCollision"));
	SphereComponent->SetEnableGravity(false);
}

UStaticMesh* ABalloon::GetActorMesh() const
{
	return BalloonMeshComponent->GetStaticMesh().Get();
}
//������ ��ȣǥ��
void ABalloon::SetNumberInWidget(int32 Num)
{
	if (UBalloonWidget* widget = Cast<UBalloonWidget>(NumberWidgetComponent->GetWidget()))
	{
		ScreenBalloonNumber = Num;
		widget->SetTextNumber(Num);
	}
}
//���� ǥ�� ����
void ABalloon::SetNumberWidgetVisible(bool bVisible)
{
	NumberWidgetComponent->SetVisibility(bVisible);
}

void ABalloon::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Todo : �浹�� ǳ���� �ȵǴ� ��찡 �ִ� ������ �׽�Ʈ �غ�����
	//�Ƹ� sphere �ݸ����� ��� �Ǵ� �ӵ��� �ʹ� ����
	//�浹 �˸�
	if (OtherActor->ActorHasTag(FName("Weapon")) && HitBalloonWithWeapon.IsBound())
	{

		HitBalloonWithWeapon.Execute(this);
		
	}
	else if (OtherActor->ActorHasTag(FName("Deadzone")) && HitBalloonWithWeapon.IsBound())
	{
		//���и� �˸��� ����
		ScreenBalloonNumber = -1;
		HitBalloonWithWeapon.Execute(this);
	}
}

