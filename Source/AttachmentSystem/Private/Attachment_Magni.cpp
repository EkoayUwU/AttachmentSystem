// Fill out your copyright notice in the Description page of Project Settings.


#include "Attachment_Magni.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetRenderingLibrary.h"

AAttachment_Magni::AAttachment_Magni()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(AttachmentMesh);

	RenderTarget2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("RenderTarget2D"));
	RenderTarget2D->SetupAttachment(Mesh);
}

void AAttachment_Magni::BeginPlay()
{
	Super::BeginPlay();

	// Initialize Timeline
	FOnTimelineFloat UpdateMovement;
	UpdateMovement.BindUFunction(this, FName("UpdateMagniMovement"));

	MagniMovement.AddInterpFloat(MovementCurve, UpdateMovement);
	//
	

	
}

void AAttachment_Magni::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Set Timeline Tick
	MagniMovement.TickTimeline(DeltaTime);
}

void AAttachment_Magni::DoAction()
{
	Super::DoAction();

	// Alternate between On / Off
	if (bFlipFlop)
	{
		bFlipFlop = false;
		MagniMovement.Reverse();
	}
	else
	{
		bFlipFlop = true;
		MagniMovement.Play();
	}
}

void AAttachment_Magni::UpdateMagniMovement(const float Alpha)
{
	Mesh->SetRelativeRotation(FMath::Lerp(FRotator(-40.f,0.f,0.f), FRotator(40.f,0.f,0.f), Alpha));
}

