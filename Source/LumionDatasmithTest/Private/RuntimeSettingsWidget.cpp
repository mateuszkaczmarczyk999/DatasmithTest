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
	if (ReflectionCheck) ReflectionCheck->OnCheckStateChanged.AddDynamic(this, &URuntimeSettingsWidget::OnReflectionCheck);
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
	SetCVars(TEXT("r.RayTracing"), IsChecked ? 1 : 0);
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

void URuntimeSettingsWidget::ToggleReflections(bool IsChecked)
{
	SetCVars(TEXT("r.Lumen.Reflections.ScreenTraces"), IsChecked ? 1 : 0);
	SetCVars(TEXT("r.Lumen.Reflections.HardwareRayTracing"), IsChecked ? 1 : 0);
}

void URuntimeSettingsWidget::OnRayTracingCheck(bool IsChecked)
{
	ToggleRayTracing(IsChecked);
}

void URuntimeSettingsWidget::OnShadowCheck(bool IsChecked)
{
	ToggleShadows(IsChecked);
}

void URuntimeSettingsWidget::OnReflectionCheck(bool IsChecked)
{
	ToggleReflections(IsChecked);
}

void URuntimeSettingsWidget::OnDatasmithConnectClick()
{
	if (UGameInstance* GI = GetGameInstance()) {
		if (UCADSyncSubsystem* CADSync = GI->GetSubsystem<UCADSyncSubsystem>()) {
			CADSync->Connect();

			FTimerHandle T;
			GetWorld()->GetTimerManager().SetTimer(T, FTimerDelegate::CreateWeakLambda(this, [this, CADSync]()
				{
					const bool bConnected = CADSync->IsConnected();
					UE_LOG(LogTemp, Display, TEXT("[DL] Connected: %s"), bConnected ? TEXT("YES") : TEXT("NO"));
					if (ConnectStatusText)
						ConnectStatusText->SetText(FText::FromString(bConnected ? TEXT("Connected") : TEXT("No sources / blocked")));
				}), 0.5f, false);

			if (ConnectStatusText)
			{
				const bool connected = CADSync->IsConnected();
				ConnectStatusText->SetText(FText::FromString(connected ? TEXT("Connected") : TEXT("Connecting …")));
			}
			return;
		}
		UE_LOG(LogTemp, Error, TEXT("[RuntimeSettingsWidget] CADSyncSubsystem not found."));
	}
}

void URuntimeSettingsWidget::OnDatasmithReSyncClick()
{
	if (UGameInstance* GI = GetGameInstance()) {
		if (UCADSyncSubsystem* CADSync = GI->GetSubsystem<UCADSyncSubsystem>()) {
			CADSync->ReSync();

			FTimerHandle T;
			GetWorld()->GetTimerManager().SetTimer(T, FTimerDelegate::CreateWeakLambda(this, [this, CADSync]()
				{
					const bool bConnected = CADSync->IsConnected();
					UE_LOG(LogTemp, Display, TEXT("[DL] Connected: %s"), bConnected ? TEXT("YES") : TEXT("NO"));
					if (ConnectStatusText)
						ConnectStatusText->SetText(FText::FromString(bConnected ? TEXT("Connected") : TEXT("No sources / blocked")));
				}), 0.5f, false);

			if (ConnectStatusText)
			{
				const bool connected = CADSync->IsConnected();
				ConnectStatusText->SetText(FText::FromString(connected ? TEXT("Connected") : TEXT("Connecting …")));
			}
			return;
		}
		UE_LOG(LogTemp, Error, TEXT("[RuntimeSettingsWidget] CADSyncSubsystem not found."));
	}
}