// Fill out your copyright notice in the Description page of Project Settings.


#include "CADSyncSubsystem.h"
#include "DatasmithRuntime.h"                   // ADatasmithRuntimeActor, FDatasmithRuntimeImportOptions
#include "DatasmithRuntimeBlueprintLibrary.h"   // UDirectLinkProxy
#include "DatasmithContentBlueprintLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DatasmithAssetUserData.h"
#include "CADDatasmithInspect.h"
#include "CADActorRegistry.h"


void UCADSyncSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
	ActorRegistry = NewObject<UCADActorRegistry>(this);
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


	// Should be called from other place
	if (Opened)
	{
		ActorRegistry->Bind();
	}
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
		ImportOptions.BuildHierarchy = EBuildHierarchyMethod::Unfiltered;
		ImportOptions.BuildCollisions = ECollisionEnabled::NoCollision;
		ImportOptions.bImportMetaData = true;

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

void UCADSyncSubsystem::ReSync()
{
    if (!GetWorld()) return;

    if (ActorRegistry)
    {
		ActorRegistry->Unbind();
    }
    if (Anchor)
	{
        Anchor->CloseConnection();
    }
    if (DirectLinkProxy)
	{
        DirectLinkProxy = nullptr;
    }

    FTimerDelegate ReopenConnection = FTimerDelegate::CreateUObject(this, &UCADSyncSubsystem::Connect);
    GetWorld()->GetTimerManager().SetTimerForNextTick(ReopenConnection);
}

bool UCADSyncSubsystem::IsConnected()
{
	if (!Anchor || !DirectLinkProxy) return false;
	return Anchor->IsConnected();
}

void UCADSyncSubsystem::Deinitialize()
{
	if (ActorRegistry)
	{
		ActorRegistry->Unbind();
	}
	if (DirectLinkProxy)
	{
		DirectLinkProxy = nullptr;
	}
    if (Anchor)
	{
		Anchor->CloseConnection();
		Anchor->Destroy();
		Anchor = nullptr;
    }
    Super::Deinitialize();
}
