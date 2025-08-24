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


static bool ReadAndWriteDatasmithMetaData(UObject* Object, TArray<TPair<FName, FString>>& OutputData) {
	if (!Object) return false;

    if (const UDatasmithAssetUserData* UserData = UDatasmithAssetUserData::GetDatasmithUserData(Object))
    {
        for (const auto& Entry : UserData->MetaData) OutputData.Emplace(Entry.Key, Entry.Value);
    }
	return OutputData.Num() > 0;
}

static bool ReadAndWriteDatasmithTags(AActor* Actor, TArray<FName>& OutputData) {
    if (!Actor) return false;

    for (const FName& Tag : Actor->Tags) OutputData.Emplace(Tag);
    return OutputData.Num() > 0;
}

static void LogActorMetaAndTagsData(AActor* Actor)
{
    if (!Actor) return;

	TArray<TPair<FName, FString>> MetaData;
	if (ReadAndWriteDatasmithMetaData(Actor, MetaData))
	{
		for (const auto& Entry : MetaData)
			UE_LOG(LogTemp, Display, TEXT("[META] %s = %s"), *Entry.Key.ToString(), *Entry.Value);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("[META] <none>"));
	}

	TArray<FName> Tags;
	if (ReadAndWriteDatasmithTags(Actor, Tags))
	{
		for (const FName& Tag : Tags)
			UE_LOG(LogTemp, Display, TEXT("[TAG] %s"), *Tag.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("[TAG] <none>"));
	}

	const FTransform Xf = Actor->GetActorTransform();
	const FString Loc = Xf.GetLocation().ToCompactString();
	const FString Rot = Xf.GetRotation().Rotator().ToString();
	const FString Scl = Xf.GetScale3D().ToCompactString();

	UE_LOG(LogTemp, Display,
		TEXT("[TRANSFORM] %s (%s)  Loc=%s  Rot=%s  Scale=%s"),
		*Actor->GetName(),
		*Actor->GetClass()->GetName(),
		*Loc, *Rot, *Scl);
}




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


	// Should be called from other place
	if (Opened) {
		if (UWorld* W = GetWorld())
		{
			if (!SpawnedActorHandler.IsValid())
				SpawnedActorHandler = W->AddOnActorSpawnedHandler(
					FOnActorSpawned::FDelegate::CreateUObject(this, &UCADSyncSubsystem::OnSpawnedActor));
			UE_LOG(LogTemp, Display, TEXT("CAD Sync Subsystem] SpawnedActorHandler Started"));
		}
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

    if (SpawnedActorHandler.IsValid())
    {
        GetWorld()->RemoveOnActorSpawnedHandler(SpawnedActorHandler);
        SpawnedActorHandler.Reset();
    }
    if (Anchor) {
        Anchor->CloseConnection();
    }
    if (DirectLinkProxy) {
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

void UCADSyncSubsystem::OnSpawnedActor(AActor* Actor)
{
	if (!Actor) return;

    GetWorld()->GetTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateWeakLambda(this, [this, Actor]()
            {
				LogActorMetaAndTagsData(Actor);
            }
		)
    );
}

void UCADSyncSubsystem::Deinitialize()
{
	if (SpawnedActorHandler.IsValid())
	{
		GetWorld()->RemoveOnActorSpawnedHandler(SpawnedActorHandler);
		SpawnedActorHandler.Reset();
	}
	if (DirectLinkProxy) {
		DirectLinkProxy = nullptr;
	}
    if (Anchor) {
		Anchor->CloseConnection();
		Anchor->Destroy();
		Anchor = nullptr;
    }
    Super::Deinitialize();
}
