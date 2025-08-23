// Fill out your copyright notice in the Description page of Project Settings.

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
