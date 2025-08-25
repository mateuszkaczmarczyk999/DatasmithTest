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
