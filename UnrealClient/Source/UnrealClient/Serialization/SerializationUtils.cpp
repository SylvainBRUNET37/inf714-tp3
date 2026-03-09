#include "SerializationUtils.h"

#include "UnrealClient/User/UserData.h"
#include "UnrealClient/User/UserSession.h"
#include "Kismet/GameplayStatics.h"

UUserData* SerializationUtils::DeserializeUserData(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) and JsonObject.IsValid())
	{
		UUserData* UserData = Cast<UUserData>(UGameplayStatics::CreateSaveGameObject(UUserData::StaticClass()));

		UserData->PlayerName = JsonObject->GetStringField(TEXT("name"));
		UserData->UserId = JsonObject->GetStringField(TEXT("id"));
		UserData->GuestToken = JsonObject->GetStringField(TEXT("guestToken"));

		UE_LOG(LogTemp, Log, TEXT("Parsed JSON successfully"));
		return UserData;
	}

	return nullptr;
}

UUserData* SerializationUtils::DeserializeUserData(const FString& SlotName, const int32 UserIndex)
{
	USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);

	return Cast<UUserData>(LoadedGame);
}

UUserSession* SerializationUtils::DeserializeUserSession(const FString& JsonString)
{
	UUserSession* UserSession = Cast<UUserSession>(
		UGameplayStatics::CreateSaveGameObject(UUserSession::StaticClass()));

	UserSession->SessionToken = JsonString;

	return UserSession;
}

UUserSession* SerializationUtils::DeserializeUserSession(const FString& SlotName, const int32 UserIndex)
{
	USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);

	return Cast<UUserSession>(LoadedGame);
}

void SerializationUtils::SaveUserData(UUserData* UserData, const FString& SlotName, const int32 UserIndex)
{
	check(UserData);

	UE_LOG(LogTemp, Log, TEXT("Saving user data : %s."), *UserData->GetClass()->GetName());

	UGameplayStatics::AsyncSaveGameToSlot(UserData, SlotName, UserIndex);
}

void SerializationUtils::SaveUserSession(UUserSession* UserSession, const FString& SlotName, const int32 UserIndex)
{
	check(UserSession);

	UE_LOG(LogTemp, Log, TEXT("Saving user session : %s."), *UserSession->GetClass()->GetName());

	UGameplayStatics::AsyncSaveGameToSlot(UserSession, SlotName, UserIndex);
}
