// Fill out your copyright notice in the Description page of Project Settings.


#include "CADActorRegistry.h"
#include "CADDatasmithInspect.h"
#include "CADLightManager.h"
#include "Engine/World.h"

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
		if (!WorldTickHandler.IsValid())
		{
			WorldTickHandler = FWorldDelegates::OnWorldPreActorTick.AddUObject(this, &UCADActorRegistry::OnWorldTick);
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
	if (WorldTickHandler.IsValid())
	{
		FWorldDelegates::OnWorldPreActorTick.Remove(WorldTickHandler);
		WorldTickHandler.Reset();
	}
}

void UCADActorRegistry::OnWorldTick(UWorld* World, ELevelTick TickType, float DeltaSeconds)
{
	if (!World) return;
	if (!WorldTickHandler.IsValid()) return;
	if (!LightManager) return;
	LightManager->OnTick(DeltaSeconds);
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

