#include "BackendSubsystem.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "BackendConfig.h"
#include "UserData.h"
#include "UserSession.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Kismet/GameplayStatics.h"

using namespace UE5Coro::Http;

UE5Coro::TCoroutine<UUserData*> UBackendSubsystem::LoginWithDeviceID()
{
	const FHttpRequestRef Request = CreateHttpRequest("Post", "users");
	
	auto [Response, bConnectedSuccessfully] = co_await ProcessAsync(Request);
	if (Response and bConnectedSuccessfully)
	{
		const FString Content = Response->GetContentAsString();
		UE_LOG(LogTemp, Log, TEXT("Response content: %s"), *Content);
		
		TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
		
		if (FJsonSerializer::Deserialize(Reader, JsonObject) and JsonObject.IsValid())
		{
			UUserData* UserData = Cast<UUserData>(UGameplayStatics::CreateSaveGameObject(UUserData::StaticClass()));
			
			UserData->PlayerName = JsonObject->GetStringField(TEXT("name"));
			UserData->UserId = JsonObject->GetStringField(TEXT("id"));
			UserData->GuestToken = JsonObject->GetStringField(TEXT("guestToken"));
			
			UE_LOG(LogTemp, Log, TEXT("Parsed JSON successfully"));
			co_return UserData;
		}

		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON"));
		
		co_return {};
	}
	
	UE_LOG(LogTemp, Error, TEXT("LoginWithDeviceID: Failed to connect to backend"));
	co_return {};
}

UE5Coro::TCoroutine<UUserSession*> UBackendSubsystem::CreateSession(const FString& UserId, const FString& GuestToken)
{
	const FHttpRequestRef Request = CreateHttpRequest("Post", "users/" + UserId + "/sessions/create");
	Request->SetHeader("Content-Type", "application/x-www-form-urlencoded"); 
	FString content = FString::Printf(TEXT("guestToken=%s"), *FGenericPlatformHttp::UrlEncode(GuestToken));
	Request->SetContentAsString(content);
	
	auto [Response, bConnectedSuccessfully] = co_await ProcessAsync(Request);
	if (Response and bConnectedSuccessfully)
	{
		const FString Content = Response->GetContentAsString();
		UE_LOG(LogTemp, Log, TEXT("Response content: %s"), *Content);
		UUserSession* UserSession = Cast<UUserSession>(UGameplayStatics::CreateSaveGameObject(UUserSession::StaticClass()));
		UserSession->SessionToken = Content;
		co_return UserSession;
	}
	
	UE_LOG(LogTemp, Error, TEXT("CreateSession: Failed to connect to backend"));
	co_return {};
}

UE5Coro::TCoroutine<UUserSession*> UBackendSubsystem::RefreshSession(const FString& UserId, const FString& GuestToken, const FString& SessionToken)
{
	const FHttpRequestRef Request = CreateHttpRequest("Post", "users/" + UserId + "/sessions/refresh");
	FString authHeader = FString::Printf(TEXT("Bearer %s"), *SessionToken); 
	Request->SetHeader("Authorization", authHeader);
	
	auto [Response, bConnectedSuccessfully] = co_await ProcessAsync(Request);
	if (Response and bConnectedSuccessfully)
	{
		const FString Content = Response->GetContentAsString();
		UE_LOG(LogTemp, Log, TEXT("Response content: %s"), *Content);
		UUserSession* UserSession = Cast<UUserSession>(UGameplayStatics::CreateSaveGameObject(UUserSession::StaticClass()));
		UserSession->SessionToken = Content;
		co_return UserSession;
	}
	
	
	UE_LOG(LogTemp, Error, TEXT("LoginWithDeviceID: Failed to connect to backend"));
	co_return {};
}

FHttpRequestRef UBackendSubsystem::CreateHttpRequest(const FString& Method, const FString& Endpoint)
{
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	
	Request->SetURL(BackendConfig::GetEndpointURL(Endpoint));
	Request->SetVerb(Method);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	
	return Request;
}
