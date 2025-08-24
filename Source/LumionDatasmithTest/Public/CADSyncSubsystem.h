// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CADSyncSubsystem.generated.h"

UCLASS()
class LUMIONDATASMITHTEST_API UCADSyncSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem lifecycle
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	UFUNCTION() void Connect();
	UFUNCTION() void ReSync();
	UFUNCTION() bool IsConnected();

private:
	void SpawnAnchor();
	void InitDirectLinkProxy();
	UPROPERTY() TObjectPtr<class ADatasmithRuntimeActor> Anchor = nullptr;
	UPROPERTY() TObjectPtr<class UDirectLinkProxy> DirectLinkProxy = nullptr;
	const int32 SourceIdx = 0;

	FDelegateHandle SpawnedActorHandler;
	void OnSpawnedActor(AActor* Actor);
};
