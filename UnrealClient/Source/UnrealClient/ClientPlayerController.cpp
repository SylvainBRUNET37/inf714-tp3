#include "ClientPlayerController.h"

#include "UnrealClient/Backend/BackendSubsystem.h"
#include "UnrealClient/Serialization/SerializationUtils.h"
#include "UnrealClient/User/UserData.h"
#include "UnrealClient/User/UserSession.h"
#include "Kismet/GameplayStatics.h"

UE5Coro::TCoroutine<> AClientPlayerController::ChangeUserName(const FString& NewName) const
{
	const UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);
	
	if (co_await BackendSubsystem->ChangeUserName(UserData->UserId, UserSession->SessionToken, NewName))
	{
		UE_LOG(LogTemp, Log, TEXT("ChangeUserName: Successfully changed username to %s"), *NewName);
	}
}

UE5Coro::TCoroutine<FString> AClientPlayerController::GetUserName() const
{
	const UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);
	
	if (const auto UserName = co_await BackendSubsystem->GetUserName(UserData->UserId, UserSession->SessionToken))
	{
		co_return SerializationUtils::DeserializeUserName(*UserName);
	}
	
	co_return FString("Unknown user name");
}

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
	co_return co_await LoginAndSaveUser() and co_await CreateAndSaveSession();
}

UE5Coro::TCoroutine<bool> AClientPlayerController::RetrieveUserData()
{
	UserData = SerializationUtils::DeserializeUserData(UserDataSlotName, UserIndex);
	check(UserData);

	UE_LOG(LogTemp, Log, TEXT("Loaded UserData from save."));

	const bool bHasSession = UGameplayStatics::DoesSaveGameExist(SessionSlotName, UserIndex);

	if (bHasSession)
	{
		UserSession = SerializationUtils::DeserializeUserSession(SessionSlotName, UserIndex);
		check(UserSession);

		UE_LOG(LogTemp, Log, TEXT("Loaded UserSession from save."));
	}

	co_return co_await (bHasSession
		? RefreshAndSaveSession()
		: CreateAndSaveSession());
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
