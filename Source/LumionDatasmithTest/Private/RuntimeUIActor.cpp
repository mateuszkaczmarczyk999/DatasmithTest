// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeUIActor.h"
#include "RuntimeSettingsWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void ARuntimeUIActor::BeginPlay()
{
	Super::BeginPlay();

    if (!WidgetClass) return;                                      // ensure class set in details panel

    if (UWorld* World = GetWorld())
    {
        // Create the widget for local player 0:
        WidgetInstance = CreateWidget<URuntimeSettingsWidget>(World, WidgetClass);

        if (WidgetInstance)
        {
            WidgetInstance->AddToViewport(10);                     // add to viewport with Z-order 10
        }

        // Make sure we can click UI:
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
        {
            PC->bShowMouseCursor = true;                           // show cursor for UI
            FInputModeGameAndUI Mode;                              // allow both game + UI input
            Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            Mode.SetHideCursorDuringCapture(false);
            PC->SetInputMode(Mode);                                // apply input mode
        }
    }
}
