#pragma once

#include "CoreMinimal.h"
#include "UserData.h"
#include "GameFramework/GameModeBase.h"
#include "UE5Coro.h"

#include "ClientGameMode.generated.h"

class UUserSession;

UCLASS()
class UNREALCLIENT_API AClientGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite)
	UUserData* UserData;
	
	UPROPERTY(BlueprintReadWrite)
	UUserSession* UserSession;
	
	/*
	UE5Coro::TCoroutine<> LoginWithDeviceID();
	
	void LoadAccountFromSave(const FString& SlotName, const int32& UserIndex);
	void LoadSessionFromSave(const FString& SlotName, const int32& UserIndex);

	void OnSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	
	UE5Coro::TCoroutine<> BeginPlayAsync();
	void DisplayLoginState(const bool& bLoginSucceeded) const;
	void SaveData(USaveGame* Data, const FString& SlotName, int32 UserIndex);

	static void LogUserData(const UUserData* Data);
	*/
	
	UE5Coro::TCoroutine<> BeginPlayAsync();
	
	virtual void BeginPlay() override;
	[[nodiscard]] UE5Coro::TCoroutine<bool> LoginWithDeviceID(const FString& UserDataSlotName, const int32 UserIndex);
	[[nodiscard]] UE5Coro::TCoroutine<bool> CreateSession(const FString& SessionSlotName, const int32 UserIndex);
};
