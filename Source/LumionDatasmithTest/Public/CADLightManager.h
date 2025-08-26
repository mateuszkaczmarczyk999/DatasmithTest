/*
* Class:
*   UCADLightManager handles the processing of Datasmith light proxy actors and transforms their metadata
*   into functional light components in the scene.
*
* Responsibilities:
*   - Process incoming light proxy actors and enqueue their descriptions (ProcessLightProxy).
*   - Update lights each tick based on the processing queue (OnTick).
*   - Parse light metadata into structured descriptions (ReadDescriptionFromProxy).
*   - Spawn corresponding Unreal light actors (SpawnLight).
*   - Hide proxy meshes once processed (HideProxyMesh).
*
* Threading Model:
*   - Metadata parsing and light description creation are performed on a worker thread.
*   - Uses LightDescription POD struct to store parsed light data.
*   - Actual light actor spawning and scene updates run on the Game Thread.
*/

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
	void ProcessLightProxy(AActor* Proxy);
	void OnTick(float DT);

private:
	static const FName LightProxyReady;
	static const FName LightProxyParsing;
	
	TQueue<TPair<TWeakObjectPtr<AActor>, LightDescription>, EQueueMode::Mpsc> LightDescriptionQueue;
	LightDescription ReadDescriptionFromProxy(const TMap<FName, FString>& MetaData);
	AActor* SpawnLight(AActor* Proxy, const LightDescription& Description);
	void HideProxyMesh(AActor* Proxy);
};
