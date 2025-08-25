// Fill out your copyright notice in the Description page of Project Settings.


#include "CADLightManager.h"
#include "Engine/RectLight.h"
#include "Components/RectLightComponent.h"
#include "Engine/PointLight.h"
#include "Components/PointLightComponent.h"
#include "Math/UnrealMathUtility.h"
#include "CADDatasmithInspect.h"


void UCADLightManager::OnActorSpawned(AActor* Proxy)
{
	if (!Proxy) return;

	const FName LigthProxyChecked(TEXT("Rhino_Light_Proxy_Exist"));
	if (Proxy->ActorHasTag(LigthProxyChecked)) return;

	LightDescription Description = ReadDescriptionFromProxy(Proxy);
	if (Description.TypeId == 0 || Description.TypeId == 1)
	{
		AActor* Light = SpawnLight(Proxy, Description);
		if (!Light) return;

		USceneComponent* Root = Proxy->GetRootComponent();
		Light->AttachToComponent(Root, FAttachmentTransformRules::KeepWorldTransform);
		Proxy->Tags.AddUnique(LigthProxyChecked);
		HideProxyMesh(Proxy);
	}
}

LightDescription UCADLightManager::ReadDescriptionFromProxy(AActor* Proxy)
{
	LightDescription Description;

	int32 LightTypeId = -1;
	const FString LightTypeHash = CADDatasmithInspect::FindValueFromMetaKey(Proxy, TEXT("UE_LightType"));
	bool LTypeValid = CADDatasmithInspect::ParseInt(LightTypeHash, LightTypeId);

	FLinearColor LightColor = FLinearColor::White;
	const FString LightColorHash = CADDatasmithInspect::FindValueFromMetaKey(Proxy, TEXT("UE_LightColor"));
	bool lColorValid = CADDatasmithInspect::ParseColor(LightColorHash, LightColor);

	float LightIntensity = 1000.0f;
	const FString LightIntensityHash = CADDatasmithInspect::FindValueFromMetaKey(Proxy, TEXT("UE_LightIntensity"));
	bool lIntensityValid = CADDatasmithInspect::ParseFloat(LightIntensityHash, LightIntensity);

	TArray<float> LightSize = { 100.0f, 100.0f };
	const FString LightSizeHash = CADDatasmithInspect::FindValueFromMetaKey(Proxy, TEXT("UE_LightSize"));
	bool lSizeValid = CADDatasmithInspect::ParseVector(LightSizeHash, LightSize);

	Description.Transform = Proxy->GetTransform();
	Description.TypeId = LightTypeId;
	Description.Color = LightColor;
	Description.Intensity = LightIntensity;
	Description.Size = LightSize;

	if (!LTypeValid || !lColorValid || !lIntensityValid || !lSizeValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CAD Light Manager] Invalid light description for actor %s"), *Proxy->GetName());
		Description.TypeId = -1;
	}

	return Description;
}

AActor* UCADLightManager::SpawnLight(AActor* Proxy, const LightDescription& Description)
{
	if (!Proxy) return nullptr;

	if (Description.TypeId == 0) // Point Light
	{
		APointLight* A = GetWorld()->SpawnActor<APointLight>(APointLight::StaticClass(), Description.Transform);
		TObjectPtr<UPointLightComponent> Component = A->PointLightComponent;
		Component->SetIntensityUnits(ELightUnits::Lumens);
		Component->SetIntensity(Description.Intensity);
		Component->SetLightColor(Description.Color);
		Component->SetAttenuationRadius(FMath::Max(Description.Size[0], Description.Size[1]));
		return A;
	}
	else if (Description.TypeId == 1) // Rect Light
	{
		ARectLight* A = GetWorld()->SpawnActor<ARectLight>(ARectLight::StaticClass(), Description.Transform);
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
