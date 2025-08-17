// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_Smith.h"

#include "Attachment_Base.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APawn_Smith::APawn_Smith()
{
    PrimaryActorTick.bCanEverTick = true;
	//
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bDoCollisionTest = false;
	
	// //
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

}


void APawn_Smith::BeginPlay()
{
	Super::BeginPlay();
	
	// Get Player Controller + Show Mouse Cursor
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetShowMouseCursor(true);

	if(PlayerController)
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Spawn First Attachment (Magni)
	CurrentAttachment = GetWorld()->SpawnActor<AAttachment_Base>(FirstAttachmentClass);
}


void APawn_Smith::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Gather Mouse Position in Local & World Space
	float NewX;
	float NewY;
	PlayerController->GetMousePosition(NewX, NewY);
	LocalMouseLocation = FVector2D(NewX, NewY);

	PlayerController->DeprojectMousePositionToWorld(WorldSpaceMouseLocation, WorldSpaceMouseDirection);

	if (CurrentAttachment) CheckForSnapping();

}


void APawn_Smith::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APawn_Smith::OnInteract);

		EnhancedInputComponent->BindAction(ActivateRotateAction, ETriggerEvent::Started, this, &APawn_Smith::OnPressedCamRotation);
		EnhancedInputComponent->BindAction(ActivateRotateAction, ETriggerEvent::Completed, this, &APawn_Smith::OnReleasedCamRotation);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APawn_Smith::OnLook);

		EnhancedInputComponent->BindAction(ScrollAction, ETriggerEvent::Triggered, this, &APawn_Smith::OnScroll);
	}

}

#pragma region AttachmentLocation
void APawn_Smith::CheckForSnapping()
{
	FHitResult HitResult;
	TArray<TObjectPtr<AActor>> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	const FVector StartLoc = Camera->GetComponentLocation();
	
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, StartLoc.ToString());
	ValidMousePos = WorldSpaceMouseDirection + (WorldSpaceMouseDirection * SpringArm->TargetArmLength * 2.f) + StartLoc;
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, ValidMousePos.ToString());

	// Line trace to check possible collision with railings
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLoc, ValidMousePos, CurrentAttachment->Radius, TraceTypeQuery3,false, ActorsToIgnore,
		EDrawDebugTrace::None, HitResult, true);

	// Check if Hit is Detected
	if (HitResult.bBlockingHit)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString("HitObject : %s").Append(*HitResult.GetActor()->GetName()));
		// Get Hitted Railing
		if (HitRailing = Cast<AWeapon_Railing>(HitResult.GetActor()))
		{
			SnappingMousePos = HitResult.Location;
			bIsSnapping = true;
			CursorSnapping();
		}
	}
	// Is not close enough to a rail, keep free roam movement
	else
	{
		bIsSnapping = false;
		CursorRoaming();
	}
}

void APawn_Smith::CursorSnapping()
{
	// Init variables
	FVector NewAttachmentLoc = HitRailing->StartPoint->GetComponentLocation();
	const float StartY = HitRailing->StartPoint->GetComponentLocation().Y;
	const float EndY = HitRailing->EndPoint->GetComponentLocation().Y;
	//

	// Create Step Movement
	float MouseY = FMath::RoundToInt(SnappingMousePos.Y / 10.f) * 10.f;

	// Get Normalize Value between Rail's Points
	const float tempY = FMath::GetMappedRangeValueClamped(UE::Math::TVector2(StartY, EndY), UE::Math::TVector2(0.f, 1.f), MouseY);

	// Get World Value between Rail's Points
	NewAttachmentLoc.Y = FMath::Lerp(StartY, EndY, tempY);

	// // Get Mesh Size
	// FVector MeshOffsetMin;
	// FVector MeshOffsetMax;
	// CurrentAttachment->AttachmentMesh->GetLocalBounds(MeshOffsetMin, MeshOffsetMax);
	// const float MeshLength = 0.f; //MeshOffsetMax.Y - MeshOffsetMin.Y;
	// //
	
	NewAttachmentLoc.Y -= 2.5f;
	// Apply Final Location
	const float FinalY = FMath::Clamp(EndY - CurrentAttachment->BaseOffset, StartY + CurrentAttachment->BaseOffset, NewAttachmentLoc.Y);
	NewAttachmentLoc.Y = FinalY;

	FRotator NewAttachmentRot = FRotator(HitRailing->GetActorRotation().Pitch, HitRailing->GetActorRotation().Yaw, HitRailing->GetActorRotation().Roll);

	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, NewAttachmentRot.ToString());

	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString("Rail Vector : ").Append(HitRailing->GetActorUpVector().ToString()));
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString("Default Up Vector : ").Append(FVector::UpVector.ToString()));
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString("DotProduct : ").Append(FString::SanitizeFloat(FVector::DotProduct(HitRailing->GetActorUpVector(), FVector::UpVector))));

	// Force Rotation due to Rotator Limitation if Railing is Upside Down
	if (FMath::IsNearlyEqual(FVector::DotProduct(HitRailing->GetActorUpVector(), FVector::UpVector), -1.f, 0.1f))
	{
		NewAttachmentRot = FRotator(0.f, 180.f, 180.f);
	}


	
	
	AttachmentLocation = NewAttachmentLoc;
	AttachmentRotation = NewAttachmentRot;
	CurrentAttachment->SetActorLocation(NewAttachmentLoc);
	CurrentAttachment->SetActorRotation(NewAttachmentRot);

	//
}

// Set Attachment Location on Cursor
void APawn_Smith::CursorRoaming()
{
	CurrentAttachment->SetActorLocation(ValidMousePos);
}
#pragma endregion 

#pragma region Inputs
void APawn_Smith::OnInteract(const FInputActionValue& Value)
{
	
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("Clicked"));
	
	if (CurrentAttachment && bIsSnapping)
	{
		// Spawn New Actor to attach it to Railing
		TObjectPtr<AAttachment_Base> tempNewAttachment = GetWorld()->SpawnActor<AAttachment_Base>(CurrentAttachment->GetClass());
		tempNewAttachment->SetActorLocationAndRotation(AttachmentLocation, AttachmentRotation);
		tempNewAttachment->AttachToComponent(HitRailing->RailMesh, FAttachmentTransformRules::KeepWorldTransform);

		// Delete Unused Reference
		CurrentAttachment->Destroy();
		CurrentAttachment = nullptr;

		// Swap to 4x Attachment
		if (!DoOnceChangeAttachment)
		{
			DoOnceChangeAttachment = true;
			
			// Spawn First Attachment (Magni)
			CurrentAttachment = GetWorld()->SpawnActor<AAttachment_Base>(SecondAttachmentClass);
		}
	}
	
	
}

void APawn_Smith::OnPressedCamRotation(const FInputActionValue& Value)
{
	bEnableCamRotation = true;
	//GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Blue, "True");
}

void APawn_Smith::OnReleasedCamRotation(const FInputActionValue& Value)
{
	bEnableCamRotation = false;
	//GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Blue, "False");
}

void APawn_Smith::OnLook(const FInputActionValue& Value)
{
	if(bEnableCamRotation)
	{
		const FVector2D LookVector = Value.Get<FVector2D>();
		// AddControllerYawInput(LookVector.X);
		// AddControllerPitchInput(LookVector.Y);

		this->AddActorLocalRotation(FRotator(0.f, LookVector.X , 0.f));

		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FRotator(LookVector.Y * 100, LookVector.X * 100, 0.f).ToString());
	}
}

void APawn_Smith::OnScroll(const FInputActionValue& Value)
{
	float input = Value.Get<float>() * 5.f;

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::SanitizeFloat(input));

	float NewArmLength = SpringArm->TargetArmLength;

	NewArmLength -= input;
	
	SpringArm->TargetArmLength = FMath::Clamp(NewArmLength, 200.f, 500.f);
}
#pragma endregion
