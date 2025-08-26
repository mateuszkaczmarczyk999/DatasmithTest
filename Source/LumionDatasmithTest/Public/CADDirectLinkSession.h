/*
* Class:
*   UCADDirectLinkSession encapsulates the management of a Datasmith Direct Link session,
*   including connection state, proxy initialization, and anchor handling for live CAD synchronization.
*
* Responsibilities:
*   - Establish and close the Datasmith connection (Connect, Disconnect).
*   - Report the current connection state (IsConnected).
*   - Flush session (Flush).
*   - Internally manage:
*       - Anchor actor for Datasmith runtime data (SpawnAnchor).
*       - DirectLink proxy initialization (InitProxy).
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CADDirectLinkSession.generated.h"

UCLASS()
class LUMIONDATASMITHTEST_API UCADDirectLinkSession : public UObject
{
	GENERATED_BODY()

public:
	void Connect();
	void Disconnect();
	bool IsConnected() const;
	void Flush();
	
private:
	void SpawnAnchor();
	void InitProxy();

	UPROPERTY() class ADatasmithRuntimeActor* Anchor = nullptr;
	UPROPERTY() class UDirectLinkProxy* Proxy = nullptr;
	const static int32 SOURCE_IDX = 0;
};
