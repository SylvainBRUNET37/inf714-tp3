#pragma once
#include "ClientGameMode.h"

class UUserSession;
class UUserData;

namespace SerializationUtils
{
	[[nodiscard]] UUserData* DeserializeUserData(const FString& JsonString);
	[[nodiscard]] UUserData* DeserializeUserData(const FString& SlotName, const int32 UserIndex);
	
	[[nodiscard]] UUserSession* DeserializeUserSession(const FString& JsonString);
	
	void SaveUserData(UUserData* UserData, const FString& SlotName, int32 UserIndex);
	void SaveUserSession(UUserSession* UserSession, const FString& SlotName, int32 UserIndex);
};
