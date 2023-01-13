// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlumFPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config = Game)
class APlumFPSCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Pawn mesh: 1st person view (arms; seen only by self) */
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 loadedAmmo;	// 장전된 총알 수

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 ammoPool;		// 남은 총알 수

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 magazine;		// 탄창 용량

	UPROPERTY(VisibleAnywhere, Category = Ammo)
		bool isReloading;

	UPROPERTY(VisibleAnywhere, Category = Aim)
		bool isAiming;

	UPROPERTY(VisibleAnywhere, Category = Fire)
		bool isFiring;

public:
	APlumFPSCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class APlumFPSProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;

private:
	FTimerHandle reloadTimer;
	FTimerHandle fireTimer;

protected:

	/** Fires a bullet. */
	void OnFire();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_OnFire(FVector Start, FVector End);
	bool Server_OnFire_Validate(FVector Start, FVector End);
	void Server_OnFire_Implementation(FVector Start, FVector End);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multi_OnFire(FVector Start, FVector End);
	bool Multi_OnFire_Validate(FVector Start, FVector End);
	void Multi_OnFire_Implementation(FVector Start, FVector End);

	/** Reloading ammo */
	void Reload();

	void ReloadDelay();

	void Ads();

	void FullAutoFire();

	void StopFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	float TraceDistance;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};