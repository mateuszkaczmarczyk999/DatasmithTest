// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RuntimeSettingsWidget.generated.h"

UCLASS()
class LUMIONDATASMITHTEST_API URuntimeSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget)) class UCheckBox* RaytracingCheck;
	UPROPERTY(meta = (BindWidget)) class UCheckBox* ShadowCheck;
	UPROPERTY(meta = (BindWidget)) class UButton* DatasmithLightsUpdateBtn;
	UPROPERTY(meta = (BindWidget)) class UButton* DatasmithConnectBtn;
	UPROPERTY(meta = (BindWidget)) class UButton* DatasmithReSyncBtn;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* ConnectStatusText;

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION() void OnRayTracingCheck(bool IsChecked);
	UFUNCTION() void OnShadowCheck(bool IsChecked);
	UFUNCTION() void OnDatasmithLightsUpdateClick();
	UFUNCTION() void OnDatasmithConnectClick();
	UFUNCTION() void OnDatasmithReSyncClick();
	
	void SetCVars(const TCHAR* VarName, int32 VarValue);
	void ToggleRayTracing(bool IsEnabled);
	void ToggleShadows(bool IsEnabled);

	class UCADSyncSubsystem* TryGetCADSyncSystem();
	void SetConnectionStatusText(const TCHAR* Text);
	void CheckConnectionStatus();
	const float PostConnectionDelay = 0.5f;
	FTimerHandle ConnectionStatusTimerHandle;
};
