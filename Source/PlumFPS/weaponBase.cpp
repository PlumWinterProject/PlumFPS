// Fill out your copyright notice in the Description page of Project Settings.


#include "weaponBase.h"

// Sets default values
AweaponBase::AweaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AweaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AweaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

