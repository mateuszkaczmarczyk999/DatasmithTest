// Fill out your copyright notice in the Description page of Project Settings.


#include "CADSyncSubsystem.h"
#include "DatasmithRuntime.h"                   // ADatasmithRuntimeActor, FDatasmithRuntimeImportOptions
#include "DatasmithRuntimeBlueprintLibrary.h"   // UDirectLinkProxy
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UCADSyncSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
	Connect();
}

void UCADSyncSubsystem::Connect()
{
	if (!Anchor) SpawnAnchor();
	if (!DirectLinkProxy) InitDirectLinkProxy();
	if (!Anchor || !DirectLinkProxy) return;

	const TArray<FDatasmithRuntimeSourceInfo> Sources = DirectLinkProxy->GetListOfSources();
	if (Sources.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("[CAD Sync Subsystem] No sources found in DirectLinkProxy!"));
		return;
	}

	const bool Opened = Anchor->OpenConnectionWithIndex(SourceIdx);
	UE_LOG(LogTemp, Log, TEXT("[CAD Sync Subsystem] OpenConnectionWithIndex: %s"), Opened ? TEXT("Success") : TEXT("Failed"));
}

void UCADSyncSubsystem::SpawnAnchor()
{
	if (Anchor) return;
	if (UWorld* World = GetWorld()) {
		FTransform Transform = FTransform::Identity;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Name = TEXT("DatasmithAnchor");
		Anchor = World->SpawnActor<ADatasmithRuntimeActor>(ADatasmithRuntimeActor::StaticClass(), Transform, SpawnParams);

		FDatasmithRuntimeImportOptions ImportOptions;
		ImportOptions.BuildHierarchy = EBuildHierarchyMethod::Simplified;
		ImportOptions.BuildCollisions = ECollisionEnabled::NoCollision;
		ImportOptions.bImportMetaData = false;

		Anchor->ImportOptions = ImportOptions;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("[CAD Sync Subsystem] No valid world found to spawn Anchor!"));
	}
}

void UCADSyncSubsystem::InitDirectLinkProxy()
{
	if (DirectLinkProxy) return;
	DirectLinkProxy = UDatasmithRuntimeLibrary::GetDirectLinkProxy();
	if (!DirectLinkProxy) {
		UE_LOG(LogTemp, Warning, TEXT("[CAD Sync Subsystem] DirectLinkProxy is not exist!"));
	}
}

bool UCADSyncSubsystem::IsConnected()
{
	if (!Anchor || !DirectLinkProxy) return false;
	return Anchor->IsConnected();
}

void UCADSyncSubsystem::Deinitialize()
{
    if (Anchor) {
		Anchor->CloseConnection();
		Anchor->Destroy();
		Anchor = nullptr;
    }
    Super::Deinitialize();
}
