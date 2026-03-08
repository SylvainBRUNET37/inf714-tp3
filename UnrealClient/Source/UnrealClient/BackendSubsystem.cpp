#include "BackendSubsystem.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "BackendConfig.h"
#include "UserSession.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Kismet/GameplayStatics.h"

using namespace UE5Coro::Http;

UE5Coro::TCoroutine<UUserSession*> UBackendSubsystem::CreateSession(const FString& UserId, const FString& GuestToken)
{
	// const FHttpRequestRef Request = CreateHttpRequest("Post", "users/" + UserId + "/sessions/create");
	// Request->SetHeader("Content-Type", "application/x-www-form-urlencoded"); 
	// FString content = FString::Printf(TEXT("guestToken=%s"), *FGenericPlatformHttp::UrlEncode(GuestToken));
	// Request->SetContentAsString(content);
	//
	// auto [Response, bConnectedSuccessfully] = co_await ProcessAsync(Request);
	// if (Response and bConnectedSuccessfully)
	// {
	// 	const FString Content = Response->GetContentAsString();
	// 	UE_LOG(LogTemp, Log, TEXT("Response content: %s"), *Content);
	// 	UUserSession* UserSession = Cast<UUserSession>(UGameplayStatics::CreateSaveGameObject(UUserSession::StaticClass()));
	// 	UserSession->SessionToken = Content;
	// 	co_return UserSession;
	// }
	//
	// UE_LOG(LogTemp, Error, TEXT("CreateSession: Failed to connect to backend"));

	co_return {};
}

UE5Coro::TCoroutine<UUserSession*> UBackendSubsystem::RefreshSession(const FString& UserId, const FString& GuestToken,
                                                                     const FString& SessionToken)
{
	// const FHttpRequestRef Request = CreateHttpRequest("Post", "users/" + UserId + "/sessions/refresh");
	// FString authHeader = FString::Printf(TEXT("Bearer %s"), *SessionToken); 
	// Request->SetHeader("Authorization", authHeader);
	//
	// auto [Response, bConnectedSuccessfully] = co_await ProcessAsync(Request);
	// if (Response and bConnectedSuccessfully)
	// {
	// 	const FString Content = Response->GetContentAsString();
	// 	UE_LOG(LogTemp, Log, TEXT("Response content: %s"), *Content);
	// 	UUserSession* UserSession = Cast<UUserSession>(UGameplayStatics::CreateSaveGameObject(UUserSession::StaticClass()));
	// 	UserSession->SessionToken = Content;
	// 	co_return UserSession;
	// }
	//
	//
	// UE_LOG(LogTemp, Error, TEXT("LoginWithDeviceID: Failed to connect to backend"));

	co_return {};
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

FHttpRequestRef UBackendSubsystem::CreateSimpleHttpRequest(const FString& Method, const FString& Endpoint,
                                                           const FString& Content)
{
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(BackendConfig::GetEndpointURL(Endpoint));
	Request->SetVerb(Method);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	Request->SetContentAsString(Content);

	return Request;
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
