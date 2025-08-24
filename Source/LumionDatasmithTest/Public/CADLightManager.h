// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CADLightManager.generated.h"


struct LightDescription {
	FTransform Transform;
	int32 TypeId; // 0 for Point Light, 1 for Rect Light
	FLinearColor Color;
	float Intensity;
};

UCLASS()
class LUMIONDATASMITHTEST_API UCADLightManager : public UObject
{
	GENERATED_BODY()
	
public:
	void OnActorSpawned(AActor* Proxy);

private:
	LightDescription ReadDescriptionFromProxy(AActor* Proxy);
	AActor* SpawnLight(AActor* Proxy, const LightDescription& Description);
	void HideProxyMesh(AActor* Proxy);
};
