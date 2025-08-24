// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeUIActor.h"
#include "RuntimeSettingsWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void ARuntimeUIActor::BeginPlay()
{
	Super::BeginPlay();

    if (!WidgetClass)
    {
		UE_LOG(LogTemp, Warning, TEXT("WidgetClass is not set in ARuntimeUIActor!"));
		return;
	}

    if (UWorld* World = GetWorld())
    {
        WidgetInstance = CreateWidget<URuntimeSettingsWidget>(World, WidgetClass);
        if (WidgetInstance) WidgetInstance->AddToViewport(10);

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
        {
            PC->bShowMouseCursor = true;
            FInputModeGameAndUI Mode;
            Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            Mode.SetHideCursorDuringCapture(false);
            PC->SetInputMode(Mode);
        }
    }
}
