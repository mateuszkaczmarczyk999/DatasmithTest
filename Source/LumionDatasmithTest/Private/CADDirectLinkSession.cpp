// Fill out your copyright notice in the Description page of Project Settings.


#include "CADDirectLinkSession.h"
#include "DatasmithRuntime.h"
#include "DatasmithRuntimeBlueprintLibrary.h"


void UCADDirectLinkSession::Connect()
{
	if (!Anchor) SpawnAnchor();
	if (!Proxy) InitProxy();
	if (!Anchor || !Proxy) return;

	const TArray<FDatasmithRuntimeSourceInfo> Sources = Proxy->GetListOfSources();
	if (Sources.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("[CAD Sync Subsystem] No sources found in DirectLinkProxy!"));
		return;
	}

	const bool Opened = Anchor->OpenConnectionWithIndex(SOURCE_IDX);
	UE_LOG(LogTemp, Log, TEXT("[CAD Sync Subsystem] OpenConnectionWithIndex: %s"), Opened ? TEXT("Success") : TEXT("Failed"));
}

void UCADDirectLinkSession::Disconnect()
{
	if (!Anchor || !Proxy) return;
	if (Anchor) Anchor->CloseConnection();
	if (Proxy) Proxy = nullptr;
	UE_LOG(LogTemp, Log, TEXT("[CAD Direct Link Session] Connection closed."));
}

bool UCADDirectLinkSession::IsConnected() const
{
	if (!Anchor || !Proxy) return false;
	return Anchor->IsConnected();
}

void UCADDirectLinkSession::Flush()
{
	if (Proxy) Proxy = nullptr;
	if (Anchor)
	{
		Anchor->CloseConnection();
		Anchor->Destroy();
		Anchor = nullptr;
	}
	UE_LOG(LogTemp, Log, TEXT("[CAD Direct Link Session] Session flushed."));
}

void UCADDirectLinkSession::SpawnAnchor()
{
	if (Anchor) return;

	if (UWorld* World = GetWorld()) {
		FTransform Transform = FTransform::Identity;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Name = TEXT("DatasmithAnchor");
		Anchor = World->SpawnActor<ADatasmithRuntimeActor>(ADatasmithRuntimeActor::StaticClass(), Transform, SpawnParams);
		if (!Anchor) {
			UE_LOG(LogTemp, Warning, TEXT("[CAD Direct Link Session] Failed to spawn DatasmithAnchor!"));
			return;
		}

		FDatasmithRuntimeImportOptions ImportOptions;
		ImportOptions.BuildHierarchy = EBuildHierarchyMethod::Unfiltered;
		ImportOptions.BuildCollisions = ECollisionEnabled::NoCollision;
		ImportOptions.bImportMetaData = true;
		Anchor->ImportOptions = ImportOptions;

		UE_LOG(LogTemp, Log, TEXT("[CAD Direct Link Session] DatasmithAnchor initialized!"));
	}
}

void UCADDirectLinkSession::InitProxy()
{
	if (Proxy) return;
	Proxy = UDatasmithRuntimeLibrary::GetDirectLinkProxy();

	if (!Proxy) UE_LOG(LogTemp, Warning, TEXT("[CAD Direct Link Session] DirectLinkProxy is not exist!"))
	else UE_LOG(LogTemp, Log, TEXT("[CAD Direct Link Session] DirectLinkProxy initialized!"))
}
