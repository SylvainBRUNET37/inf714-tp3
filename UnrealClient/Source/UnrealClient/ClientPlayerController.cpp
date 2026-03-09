#include "ClientPlayerController.h"

#include "BackendSubsystem.h"
#include "SerializationUtils.h"
#include "UserData.h"
#include "UserSession.h"
#include "Kismet/GameplayStatics.h"

void AClientPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputModeToUIOnly();
	
	BeginPlayAsync();
}

void AClientPlayerController::SetInputModeToUIOnly()
{
	bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(nullptr);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

UE5Coro::TCoroutine<> AClientPlayerController::BeginPlayAsync()
{
	const bool bSuccessfullyLoggedIn =
		UGameplayStatics::DoesSaveGameExist(UserDataSlotName, UserIndex)
			? co_await RetrieveUserData()
			: co_await LogAndCreateSession();
	
	if (not bSuccessfullyLoggedIn)
	{
		co_return;
	}
	
	OnLoginSuccess.Broadcast();
}

UE5Coro::TCoroutine<bool> AClientPlayerController::LogAndCreateSession()
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

UE5Coro::TCoroutine<bool> AClientPlayerController::RetrieveUserData()
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

UE5Coro::TCoroutine<bool> AClientPlayerController::LoginAndSaveUser()
{
	const UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);

	const auto Response = co_await BackendSubsystem->Login();
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

UE5Coro::TCoroutine<bool> AClientPlayerController::CreateAndSaveSession()
{
	const UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);

	const auto Response = co_await BackendSubsystem->CreateSession(UserData->UserId, UserData->GuestToken);
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

UE5Coro::TCoroutine<bool> AClientPlayerController::RefreshAndSaveSession()
{
	const UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);

	const auto Response = 
		co_await BackendSubsystem->RefreshSession(UserData->UserId, UserSession->SessionToken);
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
