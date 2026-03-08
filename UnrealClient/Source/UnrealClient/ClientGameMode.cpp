#include "ClientGameMode.h"

#include "BackendSubsystem.h"
#include "SerializationUtils.h"
#include "Kismet/GameplayStatics.h"
#include "UserSession.h"
#include "GenericPlatform/GenericPlatformHttp.h"

void AClientGameMode::BeginPlay()
{
	Super::BeginPlay();

	BeginPlayAsync();
}

UE5Coro::TCoroutine<bool> AClientGameMode::LogAndCreateSession()
{
	if (not co_await LoginAndSaveUser())
	{
		co_return false;
	}

	if (not co_await CreateAndSaveSession())
	{
		co_return false;
	}

	co_return true;
}

UE5Coro::TCoroutine<bool> AClientGameMode::RetrieveUserData()
{
	UserData = SerializationUtils::DeserializeUserData(UserDataSlotName, UserIndex);
	check(UserData);

	UE_LOG(LogTemp, Log, TEXT("Loaded UserData from save."));

	if (UGameplayStatics::DoesSaveGameExist(SessionSlotName, UserIndex))
	{
		UserSession = SerializationUtils::DeserializeUserSession(SessionSlotName, UserIndex);
		check(UserSession);

		UE_LOG(LogTemp, Log, TEXT("Loaded UserSession from save."));

		if (not co_await RefreshAndSaveSession())
		{
			co_return false;
		}
	}
	else
	{
		if (not co_await CreateAndSaveSession())
		{
			co_return false;
		}
	}

	co_return true;
}

UE5Coro::TCoroutine<> AClientGameMode::BeginPlayAsync()
{
	const bool bSuccessfullyLoggedIn =
		UGameplayStatics::DoesSaveGameExist(UserDataSlotName, UserIndex)
			? co_await RetrieveUserData()
			: co_await LogAndCreateSession();
	
	if (not bSuccessfullyLoggedIn)
	{
		co_return;
	}
	
	
}

UE5Coro::TCoroutine<bool> AClientGameMode::LoginAndSaveUser()
{
	const UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);

	const FHttpRequestRef Request = BackendSubsystem->CreateSimpleHttpRequest(
		"Post",
		"users"
	);

	const auto Response =
		co_await BackendSubsystem->MakeHttpRequest(Request);

	if (not Response)
	{
		UE_LOG(LogTemp, Error, TEXT("LoginAndSaveUser: Failed to connect to backend"));
		co_return false;
	}

	UserData = SerializationUtils::DeserializeUserData(*Response);
	check(UserData);

	SerializationUtils::SaveUserData(UserData, UserDataSlotName, UserIndex);
	UE_LOG(LogTemp, Log, TEXT("LoginAndSaveUser Complete"));

	co_return true;
}

UE5Coro::TCoroutine<bool> AClientGameMode::CreateAndSaveSession()
{
	const UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);

	const FHttpRequestRef Request =
		BackendSubsystem->CreateSimpleHttpRequest(
			"Post",
			"users/" + UserData->UserId + "/sessions/create",
			FString::Printf(TEXT("guestToken=%s"), *FGenericPlatformHttp::UrlEncode(UserData->GuestToken))
		);

	const auto Response =
		co_await BackendSubsystem->MakeHttpRequest(Request);

	if (not Response)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateAndSaveSession: Failed to connect to backend"));
		co_return false;
	}

	UserSession = SerializationUtils::DeserializeUserSession(*Response);
	check(UserSession);

	SerializationUtils::SaveUserSession(UserSession, SessionSlotName, UserIndex);
	UE_LOG(LogTemp, Log, TEXT("CreateAndSaveSession Complete"));

	co_return true;
}

UE5Coro::TCoroutine<bool> AClientGameMode::RefreshAndSaveSession()
{
	const UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);

	const FString AuthHeader = FString::Printf(TEXT("Bearer %s"), *UserSession->SessionToken);
	const FHttpRequestRef Request = BackendSubsystem->CreateSimpleHttpRequest(
		"Post", "users/" + UserData->UserId + "/sessions/refresh");
	Request->SetHeader("Authorization", AuthHeader);

	const auto Response =
		co_await BackendSubsystem->MakeHttpRequest(Request);

	if (not Response)
	{
		UE_LOG(LogTemp, Error, TEXT("RefreshAndSaveSession: Failed to connect to backend"));
		co_return false;
	}

	UserSession = SerializationUtils::DeserializeUserSession(*Response);
	check(UserSession);

	SerializationUtils::SaveUserSession(UserSession, SessionSlotName, UserIndex);
	UE_LOG(LogTemp, Log, TEXT("RefreshAndSaveSession Complete"));

	co_return true;
}
