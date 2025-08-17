// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Attachment_Base.generated.h"

UCLASS()
class ATTACHMENTSYSTEM_API AAttachment_Base : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> EmptyMesh;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> AttachmentMesh;
	
	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (ToolTip = "Radius before snapping take effect"))
	float Radius;

	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (Tooltip = "Length/2 of the base to avoid standing in air"))
	float BaseOffset;

	bool IsColliding();
	
	UPROPERTY(EditAnywhere, Category = "Parameters")
	TObjectPtr<UMaterialInterface> DeniedMaterial;
protected:
	AAttachment_Base();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	bool bIsColliding {false};

	UPROPERTY()
	TObjectPtr<UMaterialInterface> SavedMaterial;
private:
	UFUNCTION()
	virtual void OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                                AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                                int32 OtherBodyIndex, bool bFromSweep,
	                                const FHitResult& SweepResult);

	UFUNCTION()
	void OnMeshEndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);


	// Array of colliding Objects
	TArray<TObjectPtr<AActor>> CollidingActors;
};
