#include "CADDatasmithInspect.h"
#include "DatasmithAssetUserData.h"
#include "GenericPlatform/GenericPlatformMisc.h"

bool CADDatasmithInspect::GetMetaSnapshotMap(UObject* Object, TMap<FName, FString>& OutputData)
{
	if (!Object) return false;

	if (const UDatasmithAssetUserData* UserData = UDatasmithAssetUserData::GetDatasmithUserData(Object))
	{
		OutputData.Reserve(UserData->MetaData.Num());
		for (const TPair<FName, FString>& Entry : UserData->MetaData)
		{
			OutputData.Add(Entry.Key, Entry.Value);
		}
	}
	return OutputData.Num() > 0;
}

bool CADDatasmithInspect::ReadAndWriteDatasmithMetaData(UObject* Object, TArray<TPair<FName, FString>>& OutputData)
{
	if (!Object) return false;

	if (const UDatasmithAssetUserData* UserData = UDatasmithAssetUserData::GetDatasmithUserData(Object))
	{
		for (const auto& Entry : UserData->MetaData) OutputData.Emplace(Entry.Key, Entry.Value);
	}
	return OutputData.Num() > 0;
}

bool CADDatasmithInspect::HasMetaKey(UObject* Object, FName KeyName)
{
	if (!Object) return false;

	if (const UDatasmithAssetUserData* UserData = UDatasmithAssetUserData::GetDatasmithUserData(Object))
	{
		return UserData->MetaData.Contains(KeyName);
	}
	return false;
}

FString CADDatasmithInspect::FindValueFromMetaKey(const TMap<FName, FString>& MetaData, FName KeyName)
{
	if (const FString* Value = MetaData.Find(KeyName)) return *Value;
	return TEXT("None");
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

bool CADDatasmithInspect::ParseInt(const FString StringValue, int32& OutValue)
{
	FString TempString = StringValue;
	TempString.TrimStartAndEndInline();
	return LexTryParseString(OutValue, *TempString);
}

bool CADDatasmithInspect::ParseFloat(const FString StringValue, float& OutValue)
{
	FString TempString = StringValue;
	TempString.TrimStartAndEndInline();
	return LexTryParseString(OutValue, *TempString);
}

bool CADDatasmithInspect::ParseColor(const FString StringValue, FLinearColor& OutColor)
{
	TArray<FString> ColorComponents;
	ColorComponents.Reserve(3);

	FString TempString = StringValue;
	TempString.TrimStartAndEndInline();
	TempString.ParseIntoArray(ColorComponents, TEXT(","), true);
	if (ColorComponents.Num() != 3) return false;

	float R, G, B;
	for (FString& Col : ColorComponents) { Col.TrimStartAndEndInline(); }

	if (!ParseFloat(ColorComponents[0], R) ||
		!ParseFloat(ColorComponents[1], G) ||
		!ParseFloat(ColorComponents[2], B))
	{
		return false;
	}

	OutColor = FLinearColor(R, G, B, 255.0f);
	return true;
}

bool CADDatasmithInspect::ParseVector(const FString StringValue, TArray<float>& OutVector, const int8 size)
{
	TArray<FString> VecComponents;
	VecComponents.Reserve(size);

	FString TempString = StringValue;
	TempString.TrimStartAndEndInline();
	TempString.ParseIntoArray(VecComponents, TEXT(","), true);
	if (VecComponents.Num() != size) return false;

	for (int i = 0; i < size; i++)
	{
		float value;
		VecComponents[i].TrimStartAndEndInline();
		if (!ParseFloat(VecComponents[i], value)) {
			OutVector.Empty();
			return false;
		}
		OutVector.Add(value);
	}

	return true;
}
