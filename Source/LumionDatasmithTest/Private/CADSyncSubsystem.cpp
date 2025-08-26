#include "CADSyncSubsystem.h"
#include "CADDatasmithInspect.h"
#include "CADActorRegistry.h"
#include "CADDirectLinkSession.h"

void UCADSyncSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
	ActorRegistry = NewObject<UCADActorRegistry>(this);
	DirectLinkSession = NewObject<UCADDirectLinkSession>(this);
}

void UCADSyncSubsystem::Connect()
{
	DirectLinkSession->Connect();
	if (DirectLinkSession->IsConnected()) ActorRegistry->Bind();
}

void UCADSyncSubsystem::ReSync()
{
    if (!GetWorld()) return;

    if (ActorRegistry)
    {
    	ActorRegistry->DestoyActors();
	    ActorRegistry->Unbind();
    }
	if (DirectLinkSession) DirectLinkSession->Disconnect();

    FTimerDelegate ReopenConnection = FTimerDelegate::CreateUObject(this, &UCADSyncSubsystem::Connect);
    GetWorld()->GetTimerManager().SetTimerForNextTick(ReopenConnection);
}

bool UCADSyncSubsystem::IsConnected()
{
	if (!DirectLinkSession) return false;
	return DirectLinkSession->IsConnected();
}

void UCADSyncSubsystem::UpdateLigths()
{
	if (!GetWorld()) return;
	if (ActorRegistry) ActorRegistry->CheckActorsForLightData();
}


void UCADSyncSubsystem::Deinitialize()
{
	if (ActorRegistry) ActorRegistry->Unbind();
	if (DirectLinkSession) DirectLinkSession->Flush();
    Super::Deinitialize();
}
