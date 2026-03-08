#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UE5Coro.h"

#include "BackendSubsystem.generated.h"

class UUserData;
class UUserSession;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHtppErrorSignature, FString, ErrorMessage);

UCLASS()
class UNREALCLIENT_API UBackendSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnHtppErrorSignature OnHttpError;

	UE5Coro::TCoroutine<UUserSession*> CreateSession(const FString& UserId, const FString& GuestToken);
	UE5Coro::TCoroutine<UUserSession*> RefreshSession(const FString& UserId, const FString& GuestToken,
	                                                  const FString& SessionToken);

	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> MakeHttpRequest(const FHttpRequestRef& Request) const;

	[[nodiscard]] static FHttpRequestRef CreateSimpleHttpRequest(
		const FString& Method, const FString& Endpoint, const FString& Content = {});

private:
	[[nodiscard]] static FString CreateErrorMessageFromRequest(const FHttpRequestRef& Request);
	[[nodiscard]] static FString CreateErrorMessageFromRequest(const FHttpRequestRef& Request,
	                                                           const TSharedPtr<IHttpResponse>& Response);
};
