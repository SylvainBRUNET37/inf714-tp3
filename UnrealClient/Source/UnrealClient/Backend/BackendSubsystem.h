#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UE5Coro.h"

#include "BackendSubsystem.generated.h"

class UUserData;
class UUserSession;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHtppErrorSignature, const FString&, ErrorMessage);

UCLASS()
class UNREALCLIENT_API UBackendSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnHtppErrorSignature OnHttpError;
	
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> Login() const;
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> CreateSession(const FString& UserId, const FString& GuestToken) const;
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> RefreshSession(const FString& UserId, const FString& SessionToken) const;
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> GetUserName(const FString& UserId, const FString& SessionToken) const;
	UE5Coro::TCoroutine<TOptional<FString>> ChangeUserName(const FString& UserId, const FString& SessionToken, const FString& NewName) const;
	
private:
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> MakeHttpRequest(const FHttpRequestRef& Request) const;
	
	[[nodiscard]] static FString CreateErrorMessageFromRequest(const FHttpRequestRef& Request);
	[[nodiscard]] static FString CreateErrorMessageFromRequest(const FHttpRequestRef& Request,
	                                                           const TSharedPtr<IHttpResponse>& Response);
};
