// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attachment_Base.h"
#include "Attachment_Magni.generated.h"

/**
 * 
 */
UCLASS()
class ATTACHMENTSYSTEM_API AAttachment_Magni : public AAttachment_Base
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;

protected:
	AAttachment_Magni();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnScopeBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnScopeEndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);
	
};
