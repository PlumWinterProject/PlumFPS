// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "weaponBase.h"
#include "mainWeapon.h"
#include "subWeapon.h"
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
		int32 loadedAmmo;

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 remainAmmo;		// remain bullet

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 magazine;		// capacity of magazine

	UPROPERTY(VisibleAnywhere, Category = Ammo)
		bool isReloading;

	UPROPERTY(VisibleAnywhere, Category = Aim)
		bool isAiming;

	UPROPERTY(VisibleAnywhere, Category = Fire)
		bool isFiring;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
		AweaponBase* mainWeapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
		AweaponBase* subWeapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
		AweaponBase* currentWeapon;

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

	// Character damage 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 DefaultHP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CharacterHP = 100;


private:
	FTimerHandle reloadTimer;
	FTimerHandle fireTimer;
	FTimerHandle meleeTimer;
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

	void Melee();

	void DelayMelee();

	void Ads();

	void FullAutoFire();

	void StopFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	float TraceDistance;

	//Take Damage
	UFUNCTION(BlueprintCallable)
	void SetCharacterHP(int32 hp);

	UFUNCTION(BlueprintCallable)
	int32 GetCharacterHP();

	/*
	UFUNCTION(BlueprintCallable)
	void TakeDamage(int32 damage);*/


	UFUNCTION(BlueprintCallable)
		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};