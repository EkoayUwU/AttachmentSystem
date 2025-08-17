// Fill out your copyright notice in the Description page of Project Settings.


#include "Attachment_Base.h"



// Sets default values
AAttachment_Base::AAttachment_Base()
{
 	PrimaryActorTick.bCanEverTick = true;

	EmptyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EmptyMesh"));
	SetRootComponent(EmptyMesh);
	
	AttachmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachmentMesh"));
	AttachmentMesh->SetupAttachment(RootComponent);

}


void AAttachment_Base::BeginPlay()
{
	Super::BeginPlay();

	SavedMaterial = AttachmentMesh->GetMaterial(0);
	// Delegate handling collision
	// if (AttachmentMesh)
	// {
	// 	AttachmentMesh->OnComponentBeginOverlap.Clear();
	// 	AttachmentMesh->OnComponentEndOverlap.Clear();
	//
	// 	AttachmentMesh->OnComponentBeginOverlap.AddDynamic(this, &AAttachment_Base::OnMeshBeginOverlap);
	// 	AttachmentMesh->OnComponentEndOverlap.AddDynamic(this, &AAttachment_Base::OnMeshEndOverlap);
	// }
}

void AAttachment_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bIsColliding ? GEngine->AddOnScreenDebugMessage(-1, 0.f , FColor::Yellow, "Colliding") : GEngine->AddOnScreenDebugMessage(-1, 0.f , FColor::Yellow, "Not Colliding");

}

void AAttachment_Base::OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	bIsColliding = true;

	CollidingActors.AddUnique(OtherActor);

	if(CollidingActors.Num() == 1)
	{
		AttachmentMesh->SetMaterial(0, DeniedMaterial);
	}
	
}

void AAttachment_Base::OnMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CollidingActors.Remove(OtherActor);

	if (CollidingActors.Num() == 0)
	{
		AttachmentMesh->SetMaterial(0, SavedMaterial);
		bIsColliding = false;
	}

}

bool AAttachment_Base::IsColliding()
{
	return bIsColliding;
}

