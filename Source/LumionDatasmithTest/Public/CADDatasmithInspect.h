// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

namespace CADDatasmithInspect
{
	bool ReadAndWriteDatasmithMetaData(UObject* Object, TArray<TPair<FName, FString>>& Out);
	bool ReadAndWriteDatasmithTags(AActor* Actor, TArray<FName>& Out);
	void LogActorMetaAndTagsData(AActor* Actor);
}
