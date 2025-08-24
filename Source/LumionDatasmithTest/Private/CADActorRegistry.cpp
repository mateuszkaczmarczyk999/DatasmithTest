// Fill out your copyright notice in the Description page of Project Settings.


#include "CADActorRegistry.h"
#include "CADDatasmithInspect.h"
#include "CADLightManager.h"

void UCADActorRegistry::Bind()
{
	if (UWorld* World = GetWorld())
	{
		LightManager = NewObject<UCADLightManager>(this);
		if (!SpawnedActorHandler.IsValid())
		{
			SpawnedActorHandler = World->AddOnActorSpawnedHandler(
				FOnActorSpawned::FDelegate::CreateUObject(this, &UCADActorRegistry::OnSpawnedActor));
		}
	}
}

void UCADActorRegistry::Unbind()
{
	if (SpawnedActorHandler.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			World->RemoveOnActorSpawnedHandler(SpawnedActorHandler);
			SpawnedActorHandler.Reset();
		}
	}
}

void UCADActorRegistry::OnSpawnedActor(AActor* Actor)
{
	if (!Actor) return;
	UE_LOG(LogTemp, Log, TEXT("CAD Actor Spawned: %s"), *Actor->GetName());

	GetWorld()->GetTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateWeakLambda(this, [this, Actor]()
			{
				CADDatasmithInspect::LogActorMetaAndTagsData(Actor);
				LightManager->OnActorSpawned(Actor);
			}
		)
	);
}
