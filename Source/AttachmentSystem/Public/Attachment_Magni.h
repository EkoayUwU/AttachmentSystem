// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attachment_Base.h"
#include "Components/TimelineComponent.h"
#include "Attachment_Magni.generated.h"

/**
 * 
 */
UCLASS()
class ATTACHMENTSYSTEM_API AAttachment_Magni : public AAttachment_Base
{
	GENERATED_BODY()
	
public:
	//==================================================
	// PROPERTIES & VARIABLES
	//==================================================
	UPROPERTY(EditAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, Category = "Mesh")
	TObjectPtr<USceneCaptureComponent2D> RenderTarget2D = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	TObjectPtr<UCurveFloat> MovementCurve;

	
	
protected:
	//==================================================
	// FUNCTIONS
	//==================================================
	AAttachment_Magni();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void DoAction() override;

private:
	//==================================================
	// PROPERTIES & VARIABLES
	//==================================================
	bool bFlipFlop {false};
	//==================================================
	// FUNCTIONS
	//==================================================
	FTimeline MagniMovement;

	UFUNCTION()
	void UpdateMagniMovement(const float Alpha);

	
	
};
