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
	UPROPERTY(meta = (BindWidget)) class UCheckBox* ReflectionCheck;
	UPROPERTY(meta = (BindWidget)) class UButton* DatasmithConnectBtn;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* ConnectStatusText;

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION() void OnRayTracingCheck(bool IsChecked);
	UFUNCTION() void OnShadowCheck(bool IsChecked);
	UFUNCTION() void OnReflectionCheck(bool IsChecked);
	UFUNCTION() void OnDatasmithConnectClick();
	
	void SetCVars(const TCHAR* VarName, int32 VarValue);

	void ToggleRayTracing(bool IsEnabled);
	void ToggleShadows(bool IsEnabled);
	void ToggleReflections(bool IsEnabled);	
};
