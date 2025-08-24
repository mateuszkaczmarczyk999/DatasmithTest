// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

namespace CADDatasmithInspect
{
	bool ReadAndWriteDatasmithMetaData(UObject* Object, TArray<TPair<FName, FString>>& Out);
	FString FindValueFromMetaKey(UObject* Object, FName KeyName);
	bool ReadAndWriteDatasmithTags(AActor* Actor, TArray<FName>& Out);
	void LogActorMetaAndTagsData(AActor* Actor);
	bool ParseInt(const FString StringValue, int32& OutValue);
	bool ParseFloat(const FString StringValue, float& OutValue);
	bool ParseColor(const FString StringValue, FLinearColor& OutColor);
}
