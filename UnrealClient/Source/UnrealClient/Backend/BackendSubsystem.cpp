#include "BackendSubsystem.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "BackendConfig.h"
#include "GenericPlatform/GenericPlatformHttp.h"

using namespace UE5Coro::Http;

UE5Coro::TCoroutine<TOptional<FString>> UBackendSubsystem::Login() const
{
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	
	Request->SetURL("users");
	Request->SetVerb("Post");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

	co_return co_await MakeHttpRequest(Request);
}

UE5Coro::TCoroutine<TOptional<FString>> UBackendSubsystem::CreateSession(const FString& UserId,
	const FString& GuestToken) const
{
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	
	Request->SetURL(BackendConfig::GetEndpointURL("users/" + UserId + "/sessions/create"));
	Request->SetVerb("Post");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	Request->SetContentAsString(
		FString::Printf(TEXT("guestToken=%s"), *FGenericPlatformHttp::UrlEncode(GuestToken)));

	co_return co_await MakeHttpRequest(Request);
}

UE5Coro::TCoroutine<TOptional<FString>> UBackendSubsystem::RefreshSession(const FString& UserId,
	const FString& SessionToken) const
{
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(BackendConfig::GetEndpointURL("users/" + UserId + "/sessions/refresh"));
	Request->SetVerb("Post");
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *SessionToken));

	co_return co_await MakeHttpRequest(Request);
}

UE5Coro::TCoroutine<TOptional<FString>> UBackendSubsystem::ChangeUserName(const FString& UserId,
	const FString& SessionToken, const FString& NewName) const
{
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	const FString EncodedName = FGenericPlatformHttp::UrlEncode(NewName);
	const FString Url = BackendConfig::GetEndpointURL(
		FString::Printf(TEXT("users/%s/name?value=%s"), *UserId, *EncodedName)
	);

	Request->SetURL(Url);
	Request->SetVerb(TEXT("PUT"));
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *SessionToken));

	co_return co_await MakeHttpRequest(Request);
}

UE5Coro::TCoroutine<TOptional<FString>> UBackendSubsystem::MakeHttpRequest(const FHttpRequestRef& Request) const
{
	auto [Response, bConnectedSuccessfully] = co_await ProcessAsync(Request);
	if (not Response or not bConnectedSuccessfully)
	{
		OnHttpError.Broadcast(CreateErrorMessageFromRequest(Request));
		co_return TOptional<FString>{};
	}

	if (not EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		OnHttpError.Broadcast(CreateErrorMessageFromRequest(Request, Response));
		co_return TOptional<FString>{};
	}

	co_return Response->GetContentAsString();
}

FString UBackendSubsystem::CreateErrorMessageFromRequest(const FHttpRequestRef& Request)
{
	return FString::Printf(TEXT("HTTP request failed: %s %s"), *Request->GetVerb(), *Request->GetURL());
}

FString UBackendSubsystem::CreateErrorMessageFromRequest(const FHttpRequestRef& Request,
                                                         const TSharedPtr<IHttpResponse>& Response)
{
	return FString::Printf(TEXT("HTTP request failed: %s %s - Response code: %d, Content: %s"),
	                       *Request->GetVerb(),
	                       *Request->GetURL(),
	                       Response->GetResponseCode(),
	                       *Response->GetContentAsString());
}
