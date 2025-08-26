/*
* Class:
*   UCADActorRegistry maintains a registry of spawned CAD actors and manages their lifecycle
*   during a Datasmith Direct Link session.
*
* Responsibilities:
*   - Bind and unbind to world delegates for actor spawn and tick events (Bind, Unbind).
*   - Track newly spawned actors and process them (OnSpawnedActor).
*   - Handle world ticks to monitor/update actors (OnWorldTick).
*   - Check registered actors for light-related metadata (CheckActorsForLightData).
*   - Destroy Datasmith related actors (DestroyActors).
*
* Collaborators:
*   - Uses UCADLightManager to apply light data extracted from actor metadata.
*/

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
