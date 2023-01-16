// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "weaponBase.h"
#include "GameFramework/Actor.h"
#include "subWeapon.generated.h"

UCLASS()
class PLUMFPS_API AsubWeapon : public AweaponBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AsubWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
