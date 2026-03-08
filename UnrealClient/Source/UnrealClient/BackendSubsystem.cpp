#include "BackendSubsystem.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "BackendConfig.h"

using namespace UE5Coro::Http;

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
