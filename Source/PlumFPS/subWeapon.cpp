// Fill out your copyright notice in the Description page of Project Settings.


#include "subWeapon.h"

// Sets default values
AsubWeapon::AsubWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	magazine = 12;
	remainAmmo = 60;
	coefYawRecoil = 1000;
	coefPitchRecoil = 500;
	zoomScale = 0.8f;
	fireRate = 0.2f;
	bulletSpread = 2000;
	damage = 40;
	canFullAutoFire = false;
}

// Called when the game starts or when spawned
void AsubWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AsubWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

