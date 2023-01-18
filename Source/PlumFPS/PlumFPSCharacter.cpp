// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlumFPSCharacter.h"
#include "PlumFPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "DrawDebugHelpers.h"
#include <random>

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// APlumFPSCharacter

APlumFPSCharacter::APlumFPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	loadedAmmo = 30;
	ammoPool = 100;
	magazine = 30;

	isReloading = false;
	isAiming = false;
	isFiring = false;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	TraceDistance = 2000.0f;
}

void APlumFPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
}

void APlumFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlumFPSCharacter::FullAutoFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlumFPSCharacter::StopFire);

	// Bind reload event
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlumFPSCharacter::Reload);

	// Bind ads event
	PlayerInputComponent->BindAction("Ads", IE_Pressed, this, &APlumFPSCharacter::Ads);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &APlumFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlumFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void APlumFPSCharacter::FullAutoFire()
{
	isFiring = true;
	OnFire();
	GetWorld()->GetTimerManager().SetTimer(fireTimer, this, &APlumFPSCharacter::OnFire, 0.2f, true); // 0.*f : 연사율, true : 반복
}

void APlumFPSCharacter::StopFire()
{
	isFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(fireTimer);
}

void APlumFPSCharacter::OnFire()
{
	if (loadedAmmo <= 0 || isReloading == true) { return; }

	loadedAmmo -= 1;
	UE_LOG(LogTemp, Log, TEXT("Current Ammo : %d / %d"), loadedAmmo, ammoPool);

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
		FHitResult hit;

		GetController()->GetPlayerViewPoint(SpawnLocation, SpawnRotation);

		FVector Start = SpawnLocation;
		FVector End;

		if (isAiming) // 조준 사격시에는 총알이 튀지않아야 함
		{
			End = Start + (SpawnRotation.Vector() * TraceDistance);

			std::random_device rd;
			std::mt19937 recoil(rd());
			std::uniform_int_distribution<int> yawRecoil(-50, 50);
			std::uniform_int_distribution<int> pitchRecoil(-100, 0);

			AddControllerYawInput(float(yawRecoil(recoil)) / 1000);
			AddControllerPitchInput(float(pitchRecoil(recoil)) / 500);
			//TODO 조준시에는 화면자체가 위로 올라가는 반동, 비조준시에는 총알이 퍼지는 범위만 변경 or 범위 변경과 함께 화면자체가 올라가게 하기
		}
		else // 비조준 사격시
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int> dis(-50, 50);
			// -50, 50, 2000 숫자 조절시 총알이 튀는 곳이 바뀜
			End = Start + (((SpawnRotation.Vector() + FVector(float(dis(gen)) / 2000, float(dis(gen)) / 2000, float(dis(gen)) / 2000)) * TraceDistance));

			std::mt19937 recoil(rd());
			std::uniform_int_distribution<int> yawRecoil(-50, 50);
			std::uniform_int_distribution<int> pitchRecoil(-100, 0);

			AddControllerYawInput(float(yawRecoil(recoil)) / 1000);
			AddControllerPitchInput(float(pitchRecoil(recoil)) / 500);
		}

		FCollisionQueryParams TraceParams;
		bool bHit = World->LineTraceSingleByChannel(hit, Start, End, ECC_Visibility, TraceParams);

		DrawDebugLine(World, Start, End, FColor::Blue, false, 2.0f);

		if (bHit)
		{
			if (bHit)
			{
				if (hit.Actor->ActorHasTag("head"))
				{
					UE_LOG(LogTemp, Log, TEXT("HeadShot!"));
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("BodyShot!"));
				}
				UE_LOG(LogTemp, Log, TEXT("%s"), *hit.GetActor()->GetName());
				DrawDebugBox(World, hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
			}
		}

		if (!HasAuthority())
		{
			Server_OnFire(Start, End);
		}
		else
		{
			Multi_OnFire(Start, End);
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

bool APlumFPSCharacter::Server_OnFire_Validate(FVector Start, FVector End)
{
	return true;
}

void APlumFPSCharacter::Server_OnFire_Implementation(FVector Start, FVector End)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_OnFire_Implementation HAS BEEN CALLED"));

	Multi_OnFire(Start, End);
}

bool APlumFPSCharacter::Multi_OnFire_Validate(FVector Start, FVector End)
{
	return true;
}

void APlumFPSCharacter::Multi_OnFire_Implementation(FVector Start, FVector End)
{
	UE_LOG(LogTemp, Warning, TEXT("Multi_OnFire_Implementation HAS BEEN CALLED"));

	if (!IsLocallyControlled())
	{
		FHitResult hit;
		FCollisionQueryParams TraceParams;
		bool bHit = GetWorld()->LineTraceSingleByChannel(hit, Start, End, ECC_Visibility, TraceParams);

		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 2.0f);

		if (bHit)
		{
			if (bHit)
			{
				if (hit.Actor->ActorHasTag("head"))
				{
					UE_LOG(LogTemp, Log, TEXT("HeadShot!"));
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("BodyShot!"));
				}
				UE_LOG(LogTemp, Log, TEXT("%s"), *hit.GetActor()->GetName());
				DrawDebugBox(GetWorld(), hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
			}
		}

		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
	}
}

void APlumFPSCharacter::Reload()
{
	if (isReloading == false && loadedAmmo != magazine && ammoPool != 0)
	{
		isReloading = true;
		UE_LOG(LogTemp, Log, TEXT("Start Reloading"));

		GetWorld()->GetTimerManager().SetTimer(reloadTimer, this, &APlumFPSCharacter::ReloadDelay, 1.0f, false);

		if (ammoPool <= 0 || loadedAmmo >= magazine) { return; }

		if (ammoPool < (magazine - loadedAmmo))
		{
			loadedAmmo = loadedAmmo + ammoPool;
			ammoPool = 0;
		}
		else
		{
			ammoPool = ammoPool - (magazine - loadedAmmo);
			loadedAmmo = magazine;
		}
	}
}

void APlumFPSCharacter::Ads()
{
	if (isReloading == false)
	{
		if (isAiming == false)
		{
			isAiming = true;
			FirstPersonCameraComponent->FieldOfView = 45.0f;
		}
		else
		{
			isAiming = false;
			FirstPersonCameraComponent->FieldOfView = 90.0f;
		}
	}
}

void APlumFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APlumFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APlumFPSCharacter::ReloadDelay()
{
	isReloading = false;
	UE_LOG(LogTemp, Log, TEXT("Reloading Complete\nCurrent Ammo : %d / %d"), loadedAmmo, ammoPool);
	GetWorldTimerManager().ClearTimer(reloadTimer);
}