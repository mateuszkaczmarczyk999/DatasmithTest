/*
* Class:
*   ARuntimeUIActor (extends AActor, serves as a world-anchored entry point).
*   Provides an in-world actor responsible for spawning and owning the runtime UI widget.
*
* Responsibilities:
*   - On BeginPlay, create an instance of the assigned settings widget (WidgetClass).
*   - Maintain a reference to the active widget (WidgetInstance).
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuntimeUIActor.generated.h"

class URuntimeSettingsWidget;

UCLASS()
class LUMIONDATASMITHTEST_API ARuntimeUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, Category = "UI") TSubclassOf<URuntimeSettingsWidget> WidgetClass;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY() URuntimeSettingsWidget* WidgetInstance = nullptr;
};
