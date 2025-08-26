/*
* Namespace:
*   CADDatasmithInspect provides utility functions for inspecting and parsing Datasmith metadata and tags.
*
* Responsibilities:
*   - Extract metadata snapshots and tags from objects/actors.
*   - Query metadata by key and retrieve values (strings, ints, floats, colors, vectors).
*   - Convert metadata into structured output (maps, arrays, key-value pairs).
*   - Log actor metadata and tags for debugging.
*/

#pragma once
#include "CoreMinimal.h"

namespace CADDatasmithInspect
{
	bool GetMetaSnapshotMap(UObject* Object, TMap<FName, FString>& OutputData);
	bool ReadAndWriteDatasmithMetaData(UObject* Object, TArray<TPair<FName, FString>>& Out);
	bool HasMetaKey(UObject* Object, FName KeyName);
	FString FindValueFromMetaKey(const TMap<FName, FString>& MetaData, FName KeyName);
	bool ReadAndWriteDatasmithTags(AActor* Actor, TArray<FName>& Out);
	void LogActorMetaAndTagsData(AActor* Actor);
	bool ParseInt(const FString StringValue, int32& OutValue);
	bool ParseFloat(const FString StringValue, float& OutValue);
	bool ParseColor(const FString StringValue, FLinearColor& OutColor);
	bool ParseVector(const FString StringValue, TArray<float>& OutVector, const int8 size = 2);
}
