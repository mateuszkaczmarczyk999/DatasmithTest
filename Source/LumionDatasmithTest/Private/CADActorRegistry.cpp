#include "CADActorRegistry.h"
#include "CADDatasmithInspect.h"
#include "CADLightManager.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

const FName UCADActorRegistry::DatasmithGeometry(TEXT("Datasmith_Geometry"));

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
				LightManager->ProcessLightProxy(Actor);
				Actor->Tags.Add(DatasmithGeometry);
			}
		)
	);
}

void UCADActorRegistry::CheckActorsForLightData()
{
	if (!LightManager) return;
	
	if (UWorld* World = GetWorld())
	{
		TArray<AActor*> Matches;
		UGameplayStatics::GetAllActorsWithTag(World, DatasmithGeometry, Matches);
		
		for (auto* Actor : Matches)
		{
			if (!IsValid(Actor)) continue;
			
			CADDatasmithInspect::LogActorMetaAndTagsData(Actor);
			LightManager->ProcessLightProxy(Actor);
		}
	}
}

void UCADActorRegistry::DestoyActors()
{
	if (!LightManager) return;
	
	if (UWorld* World = GetWorld())
	{
		TArray<AActor*> Matches;
		UGameplayStatics::GetAllActorsWithTag(World, DatasmithGeometry, Matches);
		
		for (auto* Actor : Matches)
		{
			if (IsValid(Actor)) Actor->Destroy();
		}
	}
}
