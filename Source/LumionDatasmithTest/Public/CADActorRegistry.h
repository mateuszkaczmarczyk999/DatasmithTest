// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "UObject/NoExportTypes.h"
#include "CADActorRegistry.generated.h"


UCLASS()
class LUMIONDATASMITHTEST_API UCADActorRegistry : public UObject
{
	GENERATED_BODY()

public:
	void Bind();
	void Unbind();
	void OnSpawnedActor(AActor* Actor);
	void OnWorldTick(UWorld* World, ELevelTick TickType, float DeltaSeconds);
	void CheckActorsForLightData();
	void DestoyActors();

private:
	static const FName DatasmithGeometry;
	
	FDelegateHandle SpawnedActorHandler;
	FDelegateHandle WorldTickHandler;
	UPROPERTY() TObjectPtr<class UCADLightManager> LightManager = nullptr;
};
