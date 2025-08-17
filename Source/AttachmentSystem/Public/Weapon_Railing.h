// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon_Railing.generated.h"

UCLASS()
class ATTACHMENTSYSTEM_API AWeapon_Railing : public AActor
{
	GENERATED_BODY()
	
public:	

	AWeapon_Railing();

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> RailMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> StartPoint;
	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> EndPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



};
