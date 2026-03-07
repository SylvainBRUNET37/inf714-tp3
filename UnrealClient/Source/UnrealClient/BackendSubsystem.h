#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UE5Coro.h"

#include "BackendSubsystem.generated.h"

class UUserData;
class UUserSession;

UCLASS()
class UNREALCLIENT_API UBackendSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	static UE5Coro::TCoroutine<UUserData*> LoginWithDeviceID();
	
	static UE5Coro::TCoroutine<UUserSession*> CreateSession(const FString& UserId, const FString& GuestToken);
	static UE5Coro::TCoroutine<UUserSession*> RefreshSession(const FString& UserId, const FString& GuestToken,
	                                            const FString& SessionToken);

private:
	[[nodiscard]] static FHttpRequestRef CreateHttpRequest(const FString& Method, const FString& Endpoint);
};
