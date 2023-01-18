// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "weaponBase.generated.h"

UCLASS()
class PLUMFPS_API AweaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AweaponBase();

	UPROPERTY()
		int32 magazine;

	UPROPERTY()
		int32 remainAmmo;

	UPROPERTY()
		int32 coefYawRecoil;

	UPROPERTY()
		int32 coefPitchRecoil;

	UPROPERTY()
		float zoomScale;

	UPROPERTY()
		float fireRate;

	UPROPERTY()
		int32 bulletSpread;

	UPROPERTY()
		int32 damage;

	UPROPERTY()
		bool canFullAutoFire;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
