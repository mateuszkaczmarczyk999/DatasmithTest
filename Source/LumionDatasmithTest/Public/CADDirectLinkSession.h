// Fill out your copyright notice in the Description page of Project Settings.

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
	class ADatasmithRuntimeActor* GetAnchor() const;
	
private:
	void SpawnAnchor();
	void InitProxy();

	UPROPERTY() class ADatasmithRuntimeActor* Anchor = nullptr;
	UPROPERTY() class UDirectLinkProxy* Proxy = nullptr;
	const static int32 SOURCE_IDX = 0;
};
