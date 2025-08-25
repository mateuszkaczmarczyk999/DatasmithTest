// Fill out your copyright notice in the Description page of Project Settings.


#include "CADLightManager.h"
#include "Engine/RectLight.h"
#include "Components/RectLightComponent.h"
#include "Engine/PointLight.h"
#include "Components/PointLightComponent.h"
#include "Math/UnrealMathUtility.h"
#include "CADDatasmithInspect.h"

const FName UCADLightManager::LightProxyReady(TEXT("Light_Proxy_Ready"));
const FName UCADLightManager::LightProxyParsing(TEXT("Light_Proxy_Parsing"));

void UCADLightManager::ProcessLightProxy(AActor* Proxy)
{
	if (!Proxy) return;
	
	if (Proxy->ActorHasTag(LightProxyReady) || Proxy->ActorHasTag(LightProxyParsing)) return;
	
	TMap<FName, FString> MetaData;
	if (!CADDatasmithInspect::GetMetaSnapshotMap(Proxy, MetaData)) return;
	Proxy->Tags.AddUnique(LightProxyParsing);

	TWeakObjectPtr<AActor> WeakProxy = Proxy;
	Async(EAsyncExecution::ThreadPool, [this, WeakProxy, MetaData = MoveTemp(MetaData)]()
	{
		LightDescription Description = ReadDescriptionFromProxy(MetaData);
		LightDescriptionQueue.Enqueue(TPair<TWeakObjectPtr<AActor>, LightDescription>(WeakProxy, MoveTemp(Description)));
	});
}

void UCADLightManager::OnTick(float DT)
{
	const int32 TickApplyCap = 100;
	int32 Applied = 0;
	
	TPair<TWeakObjectPtr<AActor>, LightDescription> ReadyDescription;
	while (Applied < TickApplyCap && LightDescriptionQueue.Dequeue(ReadyDescription))
	{
		AActor* Proxy = ReadyDescription.Key.Get();
		if (!Proxy) continue;
		
		if (Proxy->ActorHasTag(FName(LightProxyReady)))
		{
			Proxy->Tags.Remove(LightProxyParsing);
			continue;
		}

		if (ReadyDescription.Value.TypeId < 0)
		{
			Proxy->Tags.Remove(LightProxyParsing);
			continue;
		}

		AActor* Light = SpawnLight(Proxy, ReadyDescription.Value);
		if (!Light)
		{
			Proxy->Tags.Remove(LightProxyParsing);
			continue;
		}

		USceneComponent* Root = Proxy->GetRootComponent();
		Light->AttachToComponent(Root, FAttachmentTransformRules::KeepWorldTransform);
		
		Proxy->Tags.Remove(LightProxyParsing);
		Proxy->Tags.AddUnique(LightProxyReady);
		HideProxyMesh(Proxy);
		
		Applied++;
	}
}

LightDescription UCADLightManager::ReadDescriptionFromProxy(const TMap<FName, FString>& MetaData)
{
	LightDescription Description;

	int32 LightTypeId = -1;
	const FString LightTypeHash = CADDatasmithInspect::FindValueFromMetaKey(MetaData, TEXT("UE_LightType"));
	bool LTypeValid = CADDatasmithInspect::ParseInt(LightTypeHash, LightTypeId);

	FLinearColor LightColor = FLinearColor::White;
	const FString LightColorHash = CADDatasmithInspect::FindValueFromMetaKey(MetaData, TEXT("UE_LightColor"));
	bool lColorValid = CADDatasmithInspect::ParseColor(LightColorHash, LightColor);

	float LightIntensity = 1000.0f;
	const FString LightIntensityHash = CADDatasmithInspect::FindValueFromMetaKey(MetaData, TEXT("UE_LightIntensity"));
	bool lIntensityValid = CADDatasmithInspect::ParseFloat(LightIntensityHash, LightIntensity);

	TArray<float> LightSize = { 100.0f, 100.0f };
	const FString LightSizeHash = CADDatasmithInspect::FindValueFromMetaKey(MetaData, TEXT("UE_LightSize"));
	bool lSizeValid = CADDatasmithInspect::ParseVector(LightSizeHash, LightSize);

	Description.TypeId = LightTypeId;
	Description.Color = LightColor;
	Description.Intensity = LightIntensity;
	Description.Size = LightSize;

	if (!LTypeValid || !lColorValid || !lIntensityValid || !lSizeValid)
	{
		Description.TypeId = -1;
	}

	return Description;
}

AActor* UCADLightManager::SpawnLight(AActor* Proxy, const LightDescription& Description)
{
	if (!Proxy) return nullptr;

	if (Description.TypeId == 0) // Point Light
	{
		APointLight* A = GetWorld()->SpawnActor<APointLight>(APointLight::StaticClass(), Proxy->GetTransform());
		TObjectPtr<UPointLightComponent> Component = A->PointLightComponent;
		Component->SetIntensityUnits(ELightUnits::Lumens);
		Component->SetIntensity(Description.Intensity);
		Component->SetLightColor(Description.Color);
		Component->SetAttenuationRadius(FMath::Max(Description.Size[0], Description.Size[1]));
		return A;
	}
	else if (Description.TypeId == 1) // Rect Light
	{
		ARectLight* A = GetWorld()->SpawnActor<ARectLight>(ARectLight::StaticClass(), Proxy->GetTransform());
		TObjectPtr<URectLightComponent> Component = A->RectLightComponent;
		Component->SetIntensityUnits(ELightUnits::Lumens);
		Component->SetIntensity(Description.Intensity);
		Component->SetLightColor(Description.Color);
		Component->SetSourceWidth(Description.Size[0]);
		Component->SetSourceHeight(Description.Size[1]);
		return A;
	}
	
	return nullptr;
}

void UCADLightManager::HideProxyMesh(AActor* Proxy)
{
	if (!Proxy) return;

	UStaticMeshComponent* SMC = Proxy->FindComponentByClass<UStaticMeshComponent>();
	if (!SMC) return;

	SMC->SetVisibility(false);
	SMC->SetHiddenInGame(true);
	SMC->SetCastShadow(false);
	SMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
