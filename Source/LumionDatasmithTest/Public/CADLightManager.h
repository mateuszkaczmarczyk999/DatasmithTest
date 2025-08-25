// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CADLightManager.generated.h"


struct LightDescription {
	int32 TypeId;
	FLinearColor Color;
	float Intensity;
	TArray<float> Size;
};

UCLASS()
class LUMIONDATASMITHTEST_API UCADLightManager : public UObject
{
	GENERATED_BODY()
	
public:
	void OnActorSpawned(AActor* Proxy);

private:
	LightDescription ReadDescriptionFromProxy(const TMap<FName, FString>& MetaData);
	AActor* SpawnLight(AActor* Proxy, const LightDescription& Description);
	void HideProxyMesh(AActor* Proxy);
};
