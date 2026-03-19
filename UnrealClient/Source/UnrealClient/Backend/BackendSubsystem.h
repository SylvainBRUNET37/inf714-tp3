#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UE5Coro.h"

#include "BackendSubsystem.generated.h"

class UUserData;
class UUserSession;

DECLARE_DELEGATE_TwoParams(FGetSteamAuthTicketForWebApiResponse, bool, const FString&);

UCLASS()
class UNREALCLIENT_API UBackendSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> CreateTempUser() const;
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> GetUser(const FString& SteamAuthTicket) const;
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> CreateSteamUser(const FString& SteamAuthTicket) const;
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> CreateSession(const FString& UserId, const FString& GuestToken) const;
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> CreateSessionWithSteam(const FString& UserId, const FString& SteamAuthTicket) const;
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> RefreshSession(const FString& UserId, const FString& SessionToken) const;
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> GetUserName(const FString& UserId, const FString& SessionToken) const;
	UE5Coro::TCoroutine<TOptional<FString>> ChangeUserName(const FString& UserId, const FString& SessionToken, const FString& NewName) const;
	
	void GetSteamAuthTicketForWebApi(const FGetSteamAuthTicketForWebApiResponse& Delegate) const;
	
private:
	[[nodiscard]] UE5Coro::TCoroutine<TOptional<FString>> MakeHttpRequest(const FHttpRequestRef& Request) const;
	
	[[nodiscard]] static FString CreateErrorMessageFromRequest(const FHttpRequestRef& Request);
	[[nodiscard]] static FString CreateErrorMessageFromRequest(const FHttpRequestRef& Request,
	                                                           const TSharedPtr<IHttpResponse>& Response);
};
