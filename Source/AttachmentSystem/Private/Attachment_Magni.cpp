// Fill out your copyright notice in the Description page of Project Settings.


#include "Attachment_Magni.h"

AAttachment_Magni::AAttachment_Magni()
{
	// Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	// Mesh->SetupAttachment(AttachmentMesh);

}

void AAttachment_Magni::BeginPlay()
{
	Super::BeginPlay();

	Mesh->OnComponentBeginOverlap.Clear();
	Mesh->OnComponentEndOverlap.Clear();
	
	Mesh->OnComponentBeginOverlap.AddUniqueDynamic(this, &AAttachment_Magni::OnScopeBeginOverlap);
	Mesh->OnComponentEndOverlap.AddUniqueDynamic(this, &AAttachment_Magni::OnScopeEndOverlap);
}

void AAttachment_Magni::OnScopeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bIsColliding = true;
}

void AAttachment_Magni::OnScopeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsColliding = false;


}
