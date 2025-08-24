// Fill out your copyright notice in the Description page of Project Settings.


#include "CADDatasmithInspect.h"
#include "DatasmithAssetUserData.h"

bool CADDatasmithInspect::ReadAndWriteDatasmithMetaData(UObject* Object, TArray<TPair<FName, FString>>& OutputData)
{
	if (!Object) return false;

	if (const UDatasmithAssetUserData* UserData = UDatasmithAssetUserData::GetDatasmithUserData(Object))
	{
		for (const auto& Entry : UserData->MetaData) OutputData.Emplace(Entry.Key, Entry.Value);
	}
	return OutputData.Num() > 0;
}

bool CADDatasmithInspect::ReadAndWriteDatasmithTags(AActor* Actor, TArray<FName>& OutputData)
{
	if (!Actor) return false;

	for (const FName& Tag : Actor->Tags) OutputData.Emplace(Tag);
	return OutputData.Num() > 0;
}

void CADDatasmithInspect::LogActorMetaAndTagsData(AActor* Actor)
{
	if (!Actor) return;

	TArray<TPair<FName, FString>> MetaData;
	if (CADDatasmithInspect::ReadAndWriteDatasmithMetaData(Actor, MetaData))
	{
		for (const auto& Entry : MetaData)
			UE_LOG(LogTemp, Display, TEXT("[META] %s = %s"), *Entry.Key.ToString(), *Entry.Value);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("[META] <none>"));
	}

	TArray<FName> Tags;
	if (CADDatasmithInspect::ReadAndWriteDatasmithTags(Actor, Tags))
	{
		for (const FName& Tag : Tags)
			UE_LOG(LogTemp, Display, TEXT("[TAG] %s"), *Tag.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("[TAG] <none>"));
	}

	const FTransform Xf = Actor->GetActorTransform();
	const FString Loc = Xf.GetLocation().ToCompactString();
	const FString Rot = Xf.GetRotation().Rotator().ToString();
	const FString Scl = Xf.GetScale3D().ToCompactString();

	UE_LOG(LogTemp, Display,
		TEXT("[TRANSFORM] %s (%s)  Loc=%s  Rot=%s  Scale=%s"),
		*Actor->GetName(),
		*Actor->GetClass()->GetName(),
		*Loc, *Rot, *Scl);
}
