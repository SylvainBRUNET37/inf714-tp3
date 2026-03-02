#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UE5Coro.h"

#include "BackendSubsystem.generated.h"

class UUserData;

UCLASS()
class UNREALCLIENT_API UBackendSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	static UE5Coro::TCoroutine<UUserData*> LoginWithDeviceID();
	
private:
	[[nodiscard]] static FHttpRequestRef CreateHttpRequest(const FString& Method, const FString& Endpoint);
};
