/*
* Class:
	UCADSyncSubsystem (extends UGameInstanceSubsystem, so it persists for the lifetime of the game instance).
	It serves as a facade for managing Datasmith connections via UCADDirectLinkSession
	and encapsulates actor registration and synchronization through UCADActorRegistry.

Responsibilities:
	- Initialize and shut down CAD real-time session logic (Initialize, Deinitialize).
	- Provide callable functions to:
		- Establish a connection to the CAD source (Connect).
		- Refresh or re-synchronize CAD data (ReSync).
		- Update lights from CAD metadata (UpdateLights).
		- Report the current connection state (IsConnected).
 */

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CADSyncSubsystem.generated.h"

UCLASS()
class LUMIONDATASMITHTEST_API UCADSyncSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	UFUNCTION() void Connect();
	UFUNCTION() void ReSync();
	UFUNCTION() void UpdateLigths();
	UFUNCTION() bool IsConnected();

private:
	UPROPERTY() TObjectPtr<class UCADActorRegistry> ActorRegistry = nullptr;
	UPROPERTY() TObjectPtr<class UCADDirectLinkSession> DirectLinkSession = nullptr;
};
