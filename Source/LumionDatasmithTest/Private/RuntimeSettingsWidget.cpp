// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeSettingsWidget.h"
#include "CADSyncSubsystem.h"

#include "HAL/IConsoleManager.h"
#include "Engine/Engine.h"
#include "Engine/Light.h"
#include "EngineUtils.h"
#include "Components/LightComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CHeckbox.h"

void URuntimeSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RaytracingCheck) RaytracingCheck->OnCheckStateChanged.AddDynamic(this, &URuntimeSettingsWidget::OnRayTracingCheck);
	if (ShadowCheck) ShadowCheck->OnCheckStateChanged.AddDynamic(this, &URuntimeSettingsWidget::OnShadowCheck);
	if (DatasmithConnectBtn) DatasmithConnectBtn->OnClicked.AddDynamic(this, &URuntimeSettingsWidget::OnDatasmithConnectClick);
	if (DatasmithReSyncBtn) DatasmithReSyncBtn->OnClicked.AddDynamic(this, &URuntimeSettingsWidget::OnDatasmithReSyncClick);
}

void URuntimeSettingsWidget::SetCVars(const TCHAR* VarName, int32 VarValue)
{
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(VarName))
	{
		CVar->Set(VarValue, ECVF_SetByGameSetting);
		UE_LOG(LogTemp, Log, TEXT("CVar %s set to %d"), VarName, VarValue);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CVar %s not found!"), VarName);
	}
}

void URuntimeSettingsWidget::ToggleRayTracing(bool IsChecked)
{
	SetCVars(TEXT("r.RayTracing.Enable"), IsChecked ? 1 : 0);
}

void URuntimeSettingsWidget::ToggleShadows(bool IsChecked)
{
	SetCVars(TEXT("r.RayTracing.Shadows"), IsChecked ? 1 : 0);

	UWorld* World = GetWorld();
	if (!World) return;

	for (TActorIterator<ALight> It(World); It; ++It)
	{
		if (ULightComponent* LC = It->GetLightComponent())
		{
			LC->SetCastShadows(IsChecked);

			LC->SetCastRaytracedShadows(IsChecked
				? ECastRayTracedShadow::Enabled
				: ECastRayTracedShadow::Disabled);

			LC->MarkRenderStateDirty();
		}
	}
}

void URuntimeSettingsWidget::SetConnectionStatusText(const TCHAR* Text)
{
	if (ConnectStatusText) ConnectStatusText->SetText(FText::FromString(Text));
	else UE_LOG(LogTemp, Warning, TEXT("ConnectStatusText is not set!"));
}

void URuntimeSettingsWidget::OnRayTracingCheck(bool IsChecked)
{
	ToggleRayTracing(IsChecked);
}

void URuntimeSettingsWidget::OnShadowCheck(bool IsChecked)
{
	ToggleShadows(IsChecked);
}

UCADSyncSubsystem* URuntimeSettingsWidget::TryGetCADSyncSystem()
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		SetConnectionStatusText(TEXT("Game Instance missing..."));
		return nullptr;
	}

	UCADSyncSubsystem* CADSync = GI->GetSubsystem<UCADSyncSubsystem>();
	if (!CADSync)
	{
		SetConnectionStatusText(TEXT("CAD Sync Subsystem missing..."));
		return nullptr;
	}
	return CADSync;
}

void URuntimeSettingsWidget::CheckConnectionStatus()
{
	const auto CADSync = TryGetCADSyncSystem();
	if (!CADSync) return;

	const bool bConnected = CADSync->IsConnected();
	SetConnectionStatusText(bConnected ? TEXT("Connected") : TEXT("No sources."));
}

void URuntimeSettingsWidget::OnDatasmithConnectClick()
{
	const auto CADSync = TryGetCADSyncSystem();
	if (!CADSync) return;

	if (CADSync->IsConnected())
	{
		SetConnectionStatusText(TEXT("Already connected."));
		return;
	}
	SetConnectionStatusText(TEXT("Connecting �"));
	CADSync->Connect();

	GetWorld()->GetTimerManager().SetTimer(
		ConnectionStatusTimerHandle,
		FTimerDelegate::CreateUObject(this, &URuntimeSettingsWidget::CheckConnectionStatus),
		PostConnectionDelay,
		false);
}

void URuntimeSettingsWidget::OnDatasmithReSyncClick()
{
	const auto CADSync = TryGetCADSyncSystem();
	if (!CADSync) return;

	SetConnectionStatusText(TEXT("Resynchronization �"));
	CADSync->ReSync();

	GetWorld()->GetTimerManager().SetTimer(
		ConnectionStatusTimerHandle,
		FTimerDelegate::CreateUObject(this, &URuntimeSettingsWidget::CheckConnectionStatus),
		PostConnectionDelay,
		false);
}