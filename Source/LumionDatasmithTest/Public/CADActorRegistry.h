// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

private:
	FDelegateHandle SpawnedActorHandler;
	UPROPERTY() TObjectPtr<class UCADLightManager> LightManager = nullptr;
};
